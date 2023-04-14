/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#ifndef _OTA_MANAGER_STATE_UPDATE_H
#define _OTA_MANAGER_STATE_UPDATE_H

#include <cstring>
#include <list>
#include <mutex>
#include <sys/mman.h>
#include <thread>  // std::thread
#include "ota_manager_common.h"
#include "ota_manager_state.h"


class OtaManagerStateUpdate : public OtaManagerState {
public:
     static OtaManagerStateUpdate *GetInstance ();
    virtual ~OtaManagerStateUpdate();
    virtual uint32_t Init();
    virtual  uint32_t DeInit();
    virtual uint32_t Update();
private:
        OtaManagerStateUpdate();
        static void UpdateTask();
};

#endif//_OTA_MANAGER_STATE_UPDATE_H
