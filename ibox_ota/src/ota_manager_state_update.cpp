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
#include <memory>

#include "ota_manager_common.h"
#include "ota_manager_state_update.h"
#include "ota_manager_package_manager.h"
#include "ota_manager_event_manager.h"
#include "ota_manager_process_storage.h"
#include "ota_manager_cloud_communication.h"
#include "CUtil.h"


OtaManagerStateUpdate::OtaManagerStateUpdate()
{

}

 OtaManagerStateUpdate::~OtaManagerStateUpdate()
{

}

OtaManagerStateUpdate *OtaManagerStateUpdate::GetInstance ()
{
    static OtaManagerStateUpdate instance;
    return &instance;
}

uint32_t  OtaManagerStateUpdate::Init()
{
    LOG_INFO("OtaManagerStateUpdate Init");
    OtaManagerProcessStorage::GetInstance()->SetMainState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_UPDATE);
    return 0;
}

uint32_t  OtaManagerStateUpdate::DeInit()
{
    LOG_INFO("OtaManagerStateUpdate DeInit");
    return 0;
}

uint32_t  OtaManagerStateUpdate::Update()
{
     LOG_INFO("OtaManagerStateUpdate Update");
    std::thread pre_download_thread(OtaManagerStateUpdate::UpdateTask);
    pre_download_thread.detach(); 
    return 0;   
}

void OtaManagerStateUpdate::UpdateTask() {

    bool shellRet = false;
    if (access("/update/ibox_can_commu/", F_OK) == 0) {
        if (access("/update/ibox_can_commu_old/", F_OK) == 0) {
            EXE_SHELL_COMMAND("rm -rf /update/ibox_can_commu_old");            
        }
    }

    if (!EXE_SHELL_COMMAND("chmod 777 /update/ibox_can_commu/")) {
        goto FAILED;
    }

    OtaManagerCloudCommunication::GetInstance()->UploadProgress(20);
    //重命名原文件夹为application_back
    if (!EXE_SHELL_COMMAND("mv /update/ibox_can_commu /update/ibox_can_commu_old")) {
        goto FAILED;
    }

    OtaManagerCloudCommunication::GetInstance()->UploadProgress(50);
    //复制升级文件夹到当前目录
    if (!EXE_SHELL_COMMAND("mv /update/ota/ibox_can_commu/ /update/")) {
        EXE_SHELL_COMMAND("rm -rf /update/ota/ibox_can_commu/");
        EXE_SHELL_COMMAND("mv /update/ibox_can_commu_old /update/ibox_can_commu");
        goto FAILED;
    }

    EXE_SHELL_COMMAND("rm -rf /update/ota/ota.tar.gz");
    EXE_SHELL_COMMAND("rm -rf /update/ota/ibox_can_commu/");
    OtaManagerCloudCommunication::GetInstance()->UploadProgress(80);
    EXE_SHELL_COMMAND("sync");

    OtaManagerCloudCommunication::GetInstance()->UploadProgress(100);
    OtaManagerProcessStorage::GetInstance()->SetResultCode(OtaUpdateResultCode::OTA_UPDATE_RESULT_SUCCESS);
    OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_UPDATE_SUCCESS, nullptr);
    return;
FAILED:
    EXE_SHELL_COMMAND("rm -rf /update/ota/ota.tar.gz");
    EXE_SHELL_COMMAND("rm -rf /update/ota/ibox_can_commu/");
    EXE_SHELL_COMMAND("sync");
    OtaManagerProcessStorage::GetInstance()->SetResultCode(OtaUpdateResultCode::OTA_UPDATE_RESULT_ERROR);
    OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_UPDATE_FAILED, nullptr);
    return;
}


