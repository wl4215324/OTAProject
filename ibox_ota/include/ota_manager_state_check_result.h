/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#ifndef _OTA_MANAGER_STATE_CHECK_RESULT_H
#define _OTA_MANAGER_STATE_CHECK_RESULT_H

#include <cstring>
#include <mutex>
#include <thread>  // std::thread
#include "ota_manager_common.h"
#include "ota_manager_state.h"

class OtaManagerStateCheckReSult : public OtaManagerState {
public:
    static OtaManagerStateCheckReSult *GetInstance ();
    ~OtaManagerStateCheckReSult();
    virtual uint32_t Init();
    virtual  uint32_t DeInit();
    virtual uint32_t CheckResult();
private:
    OtaManagerStateCheckReSult();
    static void RebootTask();
};

#endif//_OTA_MANAGER_STATE_CHECK_RESULT_H
