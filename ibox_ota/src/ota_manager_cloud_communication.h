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
#include "ota_manager_common.h"
#include "ota_manager_package_manager.h"

class OtaManagerCloudCommunication {
public:
    static OtaManagerCloudCommunication* GetInstance();
    ~OtaManagerCloudCommunication();
    uint32_t Init();
    uint32_t DeInit();
    uint32_t CheckTask();
    uint32_t UploadProgress(int32_t progress);
    uint32_t UploadResult();
    uint32_t DownloadFile();
    uint32_t ConfirmTask();
    uint32_t AuthTask() ;
    bool IsNeedUpdate();
    bool IsSuccessUploadResult();
    bool IsStartDownload() ;
    bool IsAuthOkay() ;
private:
    OtaManagerCloudCommunication();
    std::string getCheckTaskContent();
    std::string getDownloadFileContent();
    std::string getUploadResultContent();
    std::string getDeviceID();
    std::string getVersion();
    std::string getSeed();
    void setCert();
    bool verifySeed(std::string encrypted_seed);
    static  size_t CheckTaskCallback(void * buffer, size_t size, size_t nmemb, void *userp);
    static size_t AuthCallback(void *buffer, size_t size, size_t nmemb, void *userp);
    static  size_t ConfirmCallback(void * buffer, size_t size, size_t nmemb, void *userp);
    static  size_t DownloadFileCallback(void * buffer, size_t size, size_t nmemb, void *userp);
    static  size_t UploadResultCallback(void * buffer, size_t size, size_t nmemb, void *userp);
    static size_t DownloadProgressCallback(void *data, double dltotal, double dlnow, double ultotal, double ulnow);
private:
    std::string public_key_path_{"/update/etc/rsa_public_2048.pem"};
    std::string version_path_{"/update/ibox_can_commu/version.txt"};
    std::string device_id_path_{"/update/etc/mqtt_service_conf.json"};
    std::string cert_root_path_{"/data/idps/rootcert.pem"};
    std::string cert_client_path_{"/update/client.pem"};
    std::string cert_key_path_{"/update/key.pem"};
    CURL *curl_handle_;
    struct curl_slist *curl_header_;
    std::string download_url_;
    bool is_need_update_;
    bool is_upload_result_;
    bool is_start_download_;
    bool is_auth_ok_;
    std::string seed_;
};

#endif //_OTA_MANAGER_CLOUD_COMMUNICATION_H_
