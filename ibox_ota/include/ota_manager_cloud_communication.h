/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

/*!********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#ifndef _OTA_MANAGER_CLOUD_COMMUNICATION_H_
#define _OTA_MANAGER_CLOUD_COMMUNICATION_H_

#include <sys/mman.h>
#include <cstring>
#include <list>
#include <mutex>
#include <thread> // std::thread

#include "json.hpp"
#include "curl/curl.h"
#include "ota_log_base.h"
#include "ota_manager_common.h"

class OtaManagerCloudCommunication : public LogBase {
public:
    static OtaManagerCloudCommunication* GetInstance();
    ~OtaManagerCloudCommunication();
    uint32_t Init();
    uint32_t DeInit();
    uint32_t CheckTask();
    uint32_t UploadProgress();
    uint32_t UploadResult();
    uint32_t DownloadFile();
    uint32_t ConfirmTask();
    bool IsNeedUpdate();
    bool IsSuccessUploadResult();
    bool IsStartDownload() ;

private:
    OtaManagerCloudCommunication();
    std::string getCheckTaskContent();
    std::string getDownloadFileContent();
    std::string getUploadProgressContent();
    std::string getUploadResultContent();
    static  size_t CheckTaskCallback(void * buffer, size_t size, size_t nmemb, void *userp);
    static  size_t ConfirmCallback(void * buffer, size_t size, size_t nmemb, void *userp);
    static  size_t DownloadFileCallback(void * buffer, size_t size, size_t nmemb, void *userp);
    static  size_t UploadResultCallback(void * buffer, size_t size, size_t nmemb, void *userp);
    static size_t DownloadProgressCallback(void *data, double dltotal, double dlnow, double ultotal, double ulnow);
private:
    CURL *curl_handle_;
    struct curl_slist *curl_header_;
    std::string download_url_;
    bool is_need_update_;
    bool is_upload_result_;
    bool is_start_download_;
};

#endif //_OTA_MANAGER_CLOUD_COMMUNICATION_H_
