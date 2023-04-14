/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#ifndef _OTA_MANAGER_STATE_PARSE_VERIFY_H
#define _OTA_MANAGER_STATE_PARSE_VERIFY_H

#include <cstring>
#include <mutex>
#include <thread>  // std::thread
#include "ota_manager_common.h"
#include "ota_manager_state.h"


class OtaManagerStateParseVerify : public OtaManagerState {
public:
    static OtaManagerStateParseVerify *GetInstance ();
    virtual ~OtaManagerStateParseVerify();
    virtual uint32_t Init();
    virtual  uint32_t DeInit();
    virtual uint32_t ParseAndVerify();
private:
    OtaManagerStateParseVerify();
};

#endif//_OTA_MANAGER_STATE_PARSE_VERIFY_H
