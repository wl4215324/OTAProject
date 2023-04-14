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
#include "ota_manager_state_parse_verify.h"
#include "ota_manager_process_storage.h"
#include "ota_manager_event_manager.h"
#include "CUtil.h"


OtaManagerStateParseVerify::OtaManagerStateParseVerify()
{

}

 OtaManagerStateParseVerify::~OtaManagerStateParseVerify()
{

}

OtaManagerStateParseVerify *OtaManagerStateParseVerify::GetInstance ()
{
    static OtaManagerStateParseVerify instance;
    return &instance;
}

uint32_t  OtaManagerStateParseVerify::Init()
{
    LOG_INFO("OtaManagerStateParseVerify Init");
    OtaManagerProcessStorage::GetInstance()->SetMainState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_PARSE_AND_VERIFY);
    return 0;
}

uint32_t  OtaManagerStateParseVerify::DeInit()
{
    LOG_INFO("OtaManagerStateParseVerify DeInit");
    return 0;
}

uint32_t  OtaManagerStateParseVerify::ParseAndVerify()
{
    LOG_INFO("OtaManagerStateParseVerify ParseAndVerify");
    if (!EXE_SHELL_COMMAND("tar xvf /update/ota/ota.tar.gz -C /update/ota/")) {
        OtaManagerProcessStorage::GetInstance()->SetResultCode(OtaUpdateResultCode::OTA_UPDATE_RESULT_ERROR);
        OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_PARSE_VERIFY_FAILED, nullptr);
    } else {
        OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventID::OTA_MANAGER_EVENT_PARSE_VERIFY_SUCCESS, nullptr);
    }
    return 0;
}
