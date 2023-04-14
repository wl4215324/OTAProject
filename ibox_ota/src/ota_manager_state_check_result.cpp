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
#include "ota_manager_state_check_result.h"
#include "ota_manager_event_manager.h"
#include "ota_manager_process_storage.h"
#include "CUtil.h"


OtaManagerStateCheckReSult::OtaManagerStateCheckReSult()
{

}

 OtaManagerStateCheckReSult::~OtaManagerStateCheckReSult()
{

}

OtaManagerStateCheckReSult *OtaManagerStateCheckReSult::GetInstance ()
{
    static OtaManagerStateCheckReSult instance;
    return &instance;
}

uint32_t  OtaManagerStateCheckReSult::Init()
{
    LOG_INFO("OtaManagerStateCheckReSult Init");
    OtaManagerProcessStorage::GetInstance()->SetMainState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_CHECK_RESULT);
    return 0;
}

uint32_t  OtaManagerStateCheckReSult::DeInit()
{
    LOG_INFO("OtaManagerStateCheckReSult DeInit");
    return 0;
}

uint32_t  OtaManagerStateCheckReSult::CheckResult()
{
     LOG_INFO("OtaManagerStateCheckReSult CheckResult");
    std::thread reboot_thread(OtaManagerStateCheckReSult::RebootTask);
    reboot_thread.detach();
    return 0;   
}

void OtaManagerStateCheckReSult::RebootTask() 
{
    if (OtaManagerProcessStorage::GetInstance()->GetSubState() <= OtaManagerSubState::OTA_MANAGER_SUB_STATE_CHECK_RESULT_REBOOT){
        OtaManagerProcessStorage::GetInstance()->SetSubState(OtaManagerSubState::OTA_MANAGER_SUB_STATE_CHECK_RESULT_VERSION);
        if (!EXE_SHELL_COMMAND("reboot")){
            OtaManagerProcessStorage::GetInstance()->SetSubState(OtaManagerSubState::OTA_MANAGER_SUB_STATE_CHECK_RESULT_REBOOT);
        }
    } else {
        //todo check result
        OtaManagerProcessStorage::GetInstance()->SetResultCode(OtaUpdateResultCode::OTA_UPDATE_RESULT_SUCCESS);
        OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_CHECK_UPDATE_RESULT_SUCCESS, nullptr);
    }
}
