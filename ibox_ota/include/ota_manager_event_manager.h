/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#ifndef _OTA_MANAGER_EVENT_MANAGER_H
#define _OTA_MANAGER_EVENT_MANAGER_H

#include <cstring>
#include <list>
#include <mutex>
#include <sys/mman.h>
#include <thread>  // std::thread
#include <map>
#include <vector>
#include <mutex>
#include <functional>
#include "ota_manager_common.h"



using OtaManagerEventCallback =  std::function<void (OtaManagerEventID , void*)>;

class OtaManagerEventManager{
public:
    static OtaManagerEventManager *GetInstance();
    ~OtaManagerEventManager();
    uint32_t RegisterEventCallback(OtaManagerModuleID moduleId, std::vector<OtaManagerEventID> eventVector, OtaManagerEventCallback callback);
    uint32_t UnRegisterEventCallback(OtaManagerModuleID moduleId);
    uint32_t TriggerEvent(OtaManagerEventID eventId, void *data);
    OtaManagerEventID FindEventFromState(OtaManagerMainState mainstate);
private:
    using  OtaManagerEventMap = std::map<OtaManagerModuleID, std::vector<OtaManagerEventID>>;
    using OtaManagerEventCallbackMap =  std::map<OtaManagerModuleID, OtaManagerEventCallback> ;
    OtaManagerEventManager();
    OtaManagerEventMap mOtaManagerEventMap;
    OtaManagerEventCallbackMap mOtaManagerEventCbMap;
    std::mutex mMutex;
};

#endif//_ADU_OTA_UPDATE_H
