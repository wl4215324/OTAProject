/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#ifndef _OTA_MANAGER_STATE_DOWNLOAD_H
#define _OTA_MANAGER_STATE_DOWNLOAD_H

#include <cstring>
#include <mutex>
#include <thread>  // std::thread
#include "ota_manager_common.h"
#include "ota_manager_state.h"


class OtaManagerStateDownload : public OtaManagerState {
public:
     static OtaManagerStateDownload *GetInstance ();
    virtual ~OtaManagerStateDownload();
    virtual uint32_t Init();
    virtual  uint32_t DeInit();
    virtual uint32_t Download();
private:
        OtaManagerStateDownload();
        static void DownloadTask();
};

#endif//_OTA_MANAGER_STATE_DOWNLOAD_H
