/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#ifndef _ZHITO_H2PU_OTA_MANAGER_H_
#define _ZHITO_H2PU_OTA_MANAGER_H_

#include <atomic>
#include <queue>
#include <functional>

#include "ota_manager_event_manager.h"
#include "ota_manager_state.h"
#include "ota_manager_state_context.h"
#include "ota_manager_common.h"


class IBOXOTAManager 
{
    public :
    IBOXOTAManager(); 
    IBOXOTAManager(IBOXOTAManager const&) = delete;
    IBOXOTAManager(IBOXOTAManager && other) noexcept = delete;
    ~IBOXOTAManager(); 

    void Init(void); 
    static  void DeInit(void); 
    void Run(void); 
    void Stop(void); 

    static void eventCallback(OtaManagerEventID eventId, void *data);

    std::atomic_bool exit_ = {false};
    static std::queue<OtaManagerEventID> received_event_que_;
    static std::mutex event_mutex_;
}; 



#endif  /*_ZHITO_OTA_MANAGER_H_*/