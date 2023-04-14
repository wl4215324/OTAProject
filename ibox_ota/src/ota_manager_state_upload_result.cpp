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
#include "ota_manager_state_upload_result.h"
#include "ota_manager_event_manager.h"
#include "ota_manager_cloud_communication.h"
#include "ota_manager_process_storage.h"
#include "CUtil.h"


OtaManagerStateUpLoadResult::OtaManagerStateUpLoadResult()
{

}

 OtaManagerStateUpLoadResult::~OtaManagerStateUpLoadResult()
{

}

OtaManagerStateUpLoadResult *OtaManagerStateUpLoadResult::GetInstance ()
{
    static OtaManagerStateUpLoadResult instance;
    return &instance;
}

uint32_t  OtaManagerStateUpLoadResult::Init()
{
    LOG_INFO("OtaManagerStateUpLoadResultInit");
    OtaManagerProcessStorage::GetInstance()->SetMainState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_UPLOAD_RESULT);
    return 0;
}

uint32_t  OtaManagerStateUpLoadResult::DeInit()
{
    LOG_INFO("OtaManagerStateUpLoadResult DeInit");
    return 0;
}

uint32_t  OtaManagerStateUpLoadResult::NotifyResult()
{
     LOG_INFO("OtaManagerStateUpLoadResult NotifyResult");
    std::thread notify_result_thread(OtaManagerStateUpLoadResult::NotifyResultTask);
    notify_result_thread.detach(); 
    return 0;   
}

void OtaManagerStateUpLoadResult::NotifyResultTask() {
    while (1) {
            bool flag = false;
            LOG_INFO("NotifyResultTask begin");
            if (0 == OtaManagerCloudCommunication::GetInstance()->UploadResult()) {
                flag = OtaManagerCloudCommunication::GetInstance()->IsSuccessUploadResult();
                if (flag) {
                    OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_UPLOAD_RESULT_SUCCESS, nullptr);
                    break;
                }
            }
        sleep(3);
    }
}