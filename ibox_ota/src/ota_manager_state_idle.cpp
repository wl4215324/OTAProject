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
#include "ota_manager_state_idle.h"
#include "ota_manager_cloud_communication.h"
#include "ota_manager_event_manager.h"
#include "ota_manager_package_manager.h"
#include "ota_manager_process_storage.h"
#include "CUtil.h"


OtaManagerStateIdle::OtaManagerStateIdle()
{

}

 OtaManagerStateIdle::~OtaManagerStateIdle()
{

}

OtaManagerStateIdle *OtaManagerStateIdle::GetInstance ()
{
    static OtaManagerStateIdle instance;
    return &instance;
}

uint32_t  OtaManagerStateIdle::Init()
{
    LOG_INFO("OtaManagerStateIdle Init");
    EXE_SHELL_COMMAND("rm -rf /update/ibox_can_commu_old/");
    EXE_SHELL_COMMAND("rm -rf /update/ota/ibox_can_commu/");
    OtaManagerProcessStorage::GetInstance()->SetMainState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_IDLE);
    while(1) {
        if (0 == OtaManagerCloudCommunication::GetInstance()->CheckTask()) {
            if (OtaManagerCloudCommunication::GetInstance()->IsNeedUpdate() == true) {
                OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_REQUEST, nullptr);
                return 0;
            }
        }
        sleep(60);
    }
    return 0;
}
uint32_t  OtaManagerStateIdle::DeInit()
{
    LOG_INFO("OtaManagerStateIdle DeInit");
    return 0;
}
