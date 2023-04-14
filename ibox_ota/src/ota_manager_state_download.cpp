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
#include "ota_manager_state_download.h"
#include "ota_manager_cloud_communication.h"
#include "ota_manager_event_manager.h"
#include "ota_manager_process_storage.h"
#include "CUtil.h"


OtaManagerStateDownload::OtaManagerStateDownload()
{

}

 OtaManagerStateDownload::~OtaManagerStateDownload()
{

}

OtaManagerStateDownload *OtaManagerStateDownload::GetInstance ()
{
    static OtaManagerStateDownload instance;
    return &instance;
}

uint32_t  OtaManagerStateDownload::Init()
{
    LOG_INFO("OtaManagerStateDownload Init");
    //OtaManagerProcessStorage::GetInstance()->SetMainState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_DOWNLOAD);
    return 0;
}

uint32_t  OtaManagerStateDownload::DeInit()
{
    LOG_INFO("OtaManagerStateDownload DeInit");
    return 0;
}

uint32_t  OtaManagerStateDownload::Download()
{
     LOG_INFO("OtaManagerStateDownload Download");
    std::thread download_thread(OtaManagerStateDownload::DownloadTask);
    download_thread.detach();
    return 0;   
}

void OtaManagerStateDownload::DownloadTask() {
    while (1) {
        if (0 == OtaManagerCloudCommunication::GetInstance()->DownloadFile()) {
            LOG_INFO("OtaManagerStateDownload success");
            OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_SUCCESS, nullptr);
            return;
        }
        // OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_SUCCESS, nullptr);
        // return;
        sleep(10);
    }
    OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_FAILED, nullptr);       
}