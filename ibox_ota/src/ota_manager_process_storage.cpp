/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
 #include <fcntl.h> 

#include "CUtil.h"
#include "ota_manager_common.h"
#include "ota_manager_process_storage.h"


OtaManagerProcessStorage* OtaManagerProcessStorage::GetInstance()  {
    static OtaManagerProcessStorage stroage_instance{};
    return &stroage_instance;
}

 OtaManagerProcessStorage::OtaManagerProcessStorage()  {
    processor_state_.main_state = OtaManagerMainState::OTA_MANAGER_MAIN_STATE_IDLE;
    processor_state_.sub_state = OtaManagerSubState::OTA_MANAGER_SUB_STATE_NONE;
    processor_state_.result_code = OtaUpdateResultCode::OTA_UPDATE_RESULT_SUCCESS;

    if (access("/update/ota/", F_OK) != 0) {
        (void)mkdir("/update/ota/", S_IRWXG);
    }

    // if (access("/update/ota/ibox_can_commu/", F_OK) != 0) {
    //     (void)mkdir("/update/ota/ibox_can_commu/", S_IRWXG);
    // }

    sw_progressors_db_name_ = "/update/ota/" + sw_progressors_db_name_;

    if (access(sw_progressors_db_name_.c_str(), F_OK) != 0) {
        LOG_WARN("%s  is not existed", sw_progressors_db_name_.c_str());
        UpdateProgressorsDb();
    }
}

 OtaManagerProcessStorage::~OtaManagerProcessStorage() {

 }

OtaManagerMainState OtaManagerProcessStorage::GetMainState()  {
    const std::lock_guard<std::mutex> ota_lock{ota_mutex_};
    return processor_state_.main_state;
}

OtaManagerSubState OtaManagerProcessStorage::GetSubState()  {
    const std::lock_guard<std::mutex> ota_lock{ota_mutex_};
    return processor_state_.sub_state;
}

OtaUpdateResultCode OtaManagerProcessStorage::GetResultCode()  {
    const std::lock_guard<std::mutex> ota_lock{ota_mutex_};
    return processor_state_.result_code;
}

void OtaManagerProcessStorage::SetMainState(const OtaManagerMainState& main_state) {
    const std::lock_guard<std::mutex> ota_lock{ota_mutex_};

    // 设置为idle状态时候，认为没有升级或者示升级成功
    if (OtaManagerMainState::OTA_MANAGER_MAIN_STATE_IDLE == main_state) {
        processor_state_.sub_state = OtaManagerSubState::OTA_MANAGER_SUB_STATE_NONE;
        processor_state_.result_code = OtaUpdateResultCode::OTA_UPDATE_RESULT_SUCCESS;
    }
    processor_state_.main_state = main_state;
    if (!UpdateProgressorsDb()) {
        LOG_ERR("SetMainState error");
    }
    return;
}

void OtaManagerProcessStorage::SetSubState(const OtaManagerSubState& sub_state) {
    const std::lock_guard<std::mutex> ota_lock{ota_mutex_};
    processor_state_.sub_state = sub_state;
    if (!UpdateProgressorsDb()) {
        LOG_ERR("SetSubState error");
    }
    return;
}

void OtaManagerProcessStorage::SetResultCode(const OtaUpdateResultCode& result_code) {
    const std::lock_guard<std::mutex> ota_lock{ota_mutex_};
    processor_state_.result_code = result_code;
    if (!UpdateProgressorsDb()) {
        LOG_ERR("SetResultCode error");
    }
    return;
}

bool OtaManagerProcessStorage::UpdateProgressorsDb() {
    LOG_INFO("update progressors db");
    
    const int32_t fd {open(sw_progressors_db_name_.c_str(), O_RDWR |  O_CREAT | O_TRUNC , S_IRWXU)};
    if (fd != -1) {
        FILE* const fp {fdopen(fd, "w+")};
        if (nullptr != fp) {
            nlohmann::json json_param;
            json_param["mainstate"] = processor_state_.main_state;
            json_param["substate"] = processor_state_.sub_state;
            json_param["resultcode"] = processor_state_.result_code;
            LOG_INFO("update progressors db sucess, mainstate: %d substate: %d result_code: %d ", static_cast<int32_t>(processor_state_.main_state), static_cast<int32_t>(processor_state_.sub_state), static_cast<int32_t>(processor_state_.result_code));
            if (fprintf(fp, json_param.dump().c_str()) < 0) {
                LOG_ERR("fprintf error");
            }

            if (0 != fflush(fp)) {
                LOG_ERR("fflush error");
            }
            if (fsync(fd) < 0) {
                LOG_ERR("fsync error");
            }

            if (fclose(fp) != 0) {
                LOG_ERR("fclose error");
            }
            return true;
        }
    }
    return false;
}

bool OtaManagerProcessStorage::LoadProgressorsDb() {
    LOG_INFO("************************* load sw_progressors_db.json *************************");
    std::ifstream file_stream;
    file_stream.open(sw_progressors_db_name_, std::ifstream::in);
    if (file_stream.is_open()) {
        nlohmann::json cjson;
        file_stream >> cjson;
        if (cjson.contains("mainstate")) {
            cjson.at("mainstate").get_to(processor_state_.main_state);
        }
        if (cjson.contains("substate")) {
            cjson.at("substate").get_to(processor_state_.sub_state);
        }
        if (cjson.contains("resultcode")) {
            cjson.at("resultcode").get_to(processor_state_.result_code);
        }
        LOG_INFO("load progressors db sucess, mainstate: %d substate: %d result_code: %d ", static_cast<int32_t>(processor_state_.main_state), static_cast<int32_t>(processor_state_.sub_state), static_cast<int32_t>(processor_state_.result_code));
        file_stream.close();
        return true;
    }
    LOG_ERR("load progressors db fail");
    return false;
}

void OtaManagerProcessStorage::Clear(){
    processor_state_.main_state = OtaManagerMainState::OTA_MANAGER_MAIN_STATE_IDLE;
    processor_state_.sub_state = OtaManagerSubState::OTA_MANAGER_SUB_STATE_NONE;
    processor_state_.result_code = OtaUpdateResultCode::OTA_UPDATE_RESULT_SUCCESS;
    UpdateProgressorsDb();
}