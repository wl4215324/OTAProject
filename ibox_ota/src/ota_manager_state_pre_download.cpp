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
#include "ota_manager_state_pre_download.h"
#include "ota_manager_cloud_communication.h"
#include "ota_manager_event_manager.h"
#include "ota_manager_process_storage.h"
#include "CUtil.h"


OtaManagerStatePreDownload::OtaManagerStatePreDownload()
{

}

 OtaManagerStatePreDownload::~OtaManagerStatePreDownload()
{

}

OtaManagerStatePreDownload *OtaManagerStatePreDownload::GetInstance ()
{
    static OtaManagerStatePreDownload instance;
    return &instance;
}

uint32_t  OtaManagerStatePreDownload::Init()
{
    LOG_INFO("OtaManagerStatePreDownload Init");
    OtaManagerProcessStorage::GetInstance()->SetMainState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_PREPARE_DOWNLOAD);
    return 0;
}

uint32_t  OtaManagerStatePreDownload::DeInit()
{
    LOG_INFO("OtaManagerStatePreDownload DeInit");
    return 0;
}

uint32_t  OtaManagerStatePreDownload::PrepareDownload()
{
    LOG_INFO("OtaManagerStatePreDownload PrepareDownload");
    std::thread pre_download_thread(OtaManagerStatePreDownload::PreDownloadTask);
    pre_download_thread.detach(); 
    return 0;
}

void OtaManagerStatePreDownload::PreDownloadTask() {
    while (1) {
        if (0 == OtaManagerCloudCommunication::GetInstance()->AuthTask()) {
                if (false == OtaManagerCloudCommunication::GetInstance()->IsAuthOkay()) {
                    OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_FAILED, nullptr);
                    return;
                }
        }
        if (0 == OtaManagerCloudCommunication::GetInstance()->ConfirmTask()) {
            if (OtaManagerCloudCommunication::GetInstance()->IsStartDownload()) {
                OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_START, nullptr);
                break;
            }
        }
        sleep(3);
    }
}
