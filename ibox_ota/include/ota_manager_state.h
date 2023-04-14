/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#ifndef _OTA_MANAGER_STATE_H
#define _OTA_MANAGER_STATE_H

#include <cstring>
#include <list>
#include <mutex>
#include <sys/mman.h>
#include <thread>  // std::thread
#include "ota_manager_common.h"

class OtaManagerState {
public:
    OtaManagerState();
    virtual ~OtaManagerState();
    virtual uint32_t Init();
    virtual  uint32_t DeInit();
    virtual uint32_t ParseAndVerify();
    virtual uint32_t PrepareDownload();
    virtual uint32_t Update();
    virtual uint32_t CheckResult();
    virtual uint32_t Download();
    virtual uint32_t NotifyResult();
};

#endif//_OTA_MANAGER_STATE_H
