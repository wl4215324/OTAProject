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

#include <sys/mman.h>

#include <unistd.h>
#include <iostream>
#include <memory>

#include "ota_manager_state_context.h"
#include "ota_manager_state_idle.h"
#include "ota_manager_state_pre_download.h"
#include "ota_manager_state_download.h"
#include "ota_manager_state_parse_verify.h"
#include "ota_manager_state_update.h"
#include "ota_manager_state_check_result.h"
#include "ota_manager_state_upload_result.h"
#include "CUtil.h"

OTAManagerStateContext::OTAManagerStateContext()
{
    ota_Manager_state_handle_[static_cast<uint8_t>(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_IDLE)] = OtaManagerStateIdle::GetInstance();
    ota_Manager_state_handle_[static_cast<uint8_t>(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_PARSE_AND_VERIFY)] = OtaManagerStateParseVerify::GetInstance();
    ota_Manager_state_handle_[static_cast<uint8_t>(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_PREPARE_DOWNLOAD)] = OtaManagerStatePreDownload::GetInstance();
    ota_Manager_state_handle_[static_cast<uint8_t>(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_DOWNLOAD)] = OtaManagerStateDownload::GetInstance();
    ota_Manager_state_handle_[static_cast<uint8_t>(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_UPDATE)] = OtaManagerStateUpdate::GetInstance();
    ota_Manager_state_handle_[static_cast<uint8_t>(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_CHECK_RESULT)] = OtaManagerStateCheckReSult::GetInstance();
    ota_Manager_state_handle_[static_cast<uint8_t>(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_UPLOAD_RESULT)] = OtaManagerStateUpLoadResult::GetInstance();
}

OTAManagerStateContext::~OTAManagerStateContext()
{

}

OTAManagerStateContext *OTAManagerStateContext ::GetInstance()
{
    static OTAManagerStateContext stateContext;
    return &stateContext;
}

OtaManagerState*OTAManagerStateContext::GetStateHandle()
{
    return ota_Manager_state_handle_[static_cast<uint8_t>(current_state_)];
}

uint32_t OTAManagerStateContext::ChangeState(OtaManagerMainState state)
{
    RETV_CHECK_VALUE(ota_Manager_state_handle_[static_cast<uint8_t>(current_state_)] == nullptr,  -1, "ota_Manager_state_handle_ %d is nullptr", static_cast<uint8_t>(current_state_));
    ota_Manager_state_handle_[static_cast<uint8_t>(current_state_)]->DeInit();
    current_state_ = state;
    RETV_CHECK_VALUE(ota_Manager_state_handle_[static_cast<uint8_t>(current_state_)] == nullptr,  -1, "ota_Manager_state_handle_ %d is nullptr", static_cast<uint8_t>(current_state_));
    ota_Manager_state_handle_[static_cast<uint8_t>(current_state_)]->Init();
    return 0;
}

