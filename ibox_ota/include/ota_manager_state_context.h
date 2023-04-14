/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#ifndef _OTA_MANAGER_STATE_CONTEXT_H_
#define _OTA_MANAGER_STATE_CONTEXT_H_

#include <mutex>
#include <thread>  // std::thread
#include <unistd.h>
#include <map>

#include "ota_manager_common.h"
#include "ota_manager_state.h"

typedef struct 
{
    OtaManagerMainState state;
    OtaManagerState * instance;
}OtaManagerStateMap;

class OTAManagerStateContext {
public:
    static OTAManagerStateContext *GetInstance();
    ~OTAManagerStateContext();
public:
    OtaManagerState *GetStateHandle();
    uint32_t ChangeState(OtaManagerMainState current);
private:
    OTAManagerStateContext();
    OtaManagerMainState current_state_;
    OtaManagerState *ota_Manager_state_handle_[static_cast<uint8_t>(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_MAX)];
};


#endif //_OTA_MANAGER_STATE_CONTEXT_H_