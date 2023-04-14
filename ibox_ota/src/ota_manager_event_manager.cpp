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
#include <algorithm>

#include "CUtil.h"
#include "ota_manager_common.h"
#include "ota_manager_event_manager.h"


OtaManagerEventManager *OtaManagerEventManager::GetInstance()
{
    static OtaManagerEventManager instance;
    return &instance;
}

OtaManagerEventManager::OtaManagerEventManager()
{

}

 OtaManagerEventManager::~OtaManagerEventManager()
{

}

uint32_t OtaManagerEventManager::RegisterEventCallback(OtaManagerModuleID moduleId, std::vector<OtaManagerEventID> eventVector, OtaManagerEventCallback callback)
{
    RETV_CHECK_VALUE(callback == nullptr, -1, "callback is nullptr");
    std::lock_guard<std::mutex>lock(mMutex);
    auto iter = mOtaManagerEventMap.find(moduleId);
    if (iter == mOtaManagerEventMap.end()) {
        (void)mOtaManagerEventMap.insert(std::make_pair(moduleId, eventVector));
    } else {
        LOG_WARN("mOtaManagerEventMap moduleid %d have been exist, please unregister first\n", static_cast<uint32_t>(moduleId));
    }
    auto cbiter = mOtaManagerEventCbMap.find(moduleId);
    if (cbiter == mOtaManagerEventCbMap.end()) {
        (void)mOtaManagerEventCbMap.insert(std::make_pair(moduleId, callback));
    } else {
        LOG_WARN("mOtaManagerEventCbMap moduleid %d have been exist, please unregister first\n", static_cast<uint32_t>(moduleId));
    }
    return 0;
}

uint32_t OtaManagerEventManager::UnRegisterEventCallback(OtaManagerModuleID moduleId)
{
    std::lock_guard<std::mutex> lock(mMutex);
    auto iter = mOtaManagerEventMap.find(moduleId);
    if (iter != mOtaManagerEventMap.end()) {
        (void)mOtaManagerEventMap.erase(moduleId);
    } else {
        LOG_WARN("mOtaManagerEventMap moduleid %d not exist\n", static_cast<uint32_t>(moduleId));
    }
    auto cbiter = mOtaManagerEventCbMap.find(moduleId);
    if (cbiter != mOtaManagerEventCbMap.end()) {
        (void)mOtaManagerEventCbMap.erase(moduleId);
    } else {
        LOG_WARN("mOtaManagerEventCbMap moduleid %d not exist\n", static_cast<uint32_t>(moduleId));
    }
    return 0;
}

uint32_t OtaManagerEventManager::TriggerEvent(OtaManagerEventID eventId, void *data)
{
    std::lock_guard<std::mutex> lock(mMutex);
    for (const auto & mapValue : mOtaManagerEventMap) {
        if (std::find(mapValue.second.begin(), mapValue.second.end(), eventId) != mapValue.second.end())
        {
            auto callbackIter = mOtaManagerEventCbMap.find(mapValue.first);
            if (callbackIter != mOtaManagerEventCbMap.end()) {
                OtaManagerEventCallback eventCallback = callbackIter->second;
                if (eventCallback != nullptr) {
                   eventCallback(eventId, data);
                } else {
                    LOG_WARN("mOtaManagerEventCbMap moduleid %d callback is  nullptr", static_cast<uint32_t>(mapValue.first));
                }
            }
        }
    }
    return 0;
}

OtaManagerEventID OtaManagerEventManager::FindEventFromState(OtaManagerMainState mainstate) {
    if (mainstate == OtaManagerMainState::OTA_MANAGER_MAIN_STATE_PARSE_AND_VERIFY) {
        return OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_SUCCESS;
    } else if (mainstate == OtaManagerMainState::OTA_MANAGER_MAIN_STATE_UPDATE) {
        return OtaManagerEventID::OTA_MANAGER_EVENT_PARSE_VERIFY_SUCCESS;
    } else if (mainstate == OtaManagerMainState::OTA_MANAGER_MAIN_STATE_CHECK_RESULT) {
        return OtaManagerEventID::OTA_MANAGER_EVENT_UPDATE_SUCCESS;
    } else if (mainstate == OtaManagerMainState::OTA_MANAGER_MAIN_STATE_UPLOAD_RESULT) {
        return OtaManagerEventID::OTA_MANAGER_EVENT_CHECK_UPDATE_RESULT_SUCCESS;
    } else if (mainstate == OtaManagerMainState::OTA_MANAGER_MAIN_STATE_IDLE) {
        return OtaManagerEventID::OTA_MANAGER_EVENT_UPLOAD_RESULT_SUCCESS;
    } else if (mainstate == OtaManagerMainState::OTA_MANAGER_MAIN_STATE_PREPARE_DOWNLOAD) {
        return OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_REQUEST; 
    } else {
        return OtaManagerEventID::OTA_MANAGER_EVENT_NONE;
    }
}
