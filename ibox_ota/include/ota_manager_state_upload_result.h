/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#ifndef _OTA_MANAGER_STATE_UPLOAD_RESULT_H
#define _OTA_MANAGER_STATE_UPLOAD_RESULT_H

#include <cstring>
#include <list>
#include <mutex>
#include <sys/mman.h>
#include <thread>  // std::thread
#include "ota_manager_common.h"
#include "ota_manager_state.h"


class OtaManagerStateUpLoadResult : public OtaManagerState {
public:
     static OtaManagerStateUpLoadResult *GetInstance ();
    virtual ~OtaManagerStateUpLoadResult();
    virtual uint32_t Init();
    virtual  uint32_t DeInit();
    virtual uint32_t NotifyResult();
private:
    OtaManagerStateUpLoadResult();
    static void NotifyResultTask();
};

#endif//_OTA_MANAGER_STATE_UPLOAD_RESULT_H
