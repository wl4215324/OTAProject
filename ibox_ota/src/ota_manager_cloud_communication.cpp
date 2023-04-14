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

#include "ota_manager_cloud_communication.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <memory>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "CUtil.h"
#include "ota_manager_common.h"
#include "ota_manager_event_manager.h"
#include "ota_manager_process_storage.h"

using namespace nlohmann;

#define OTA_FILE_NAME "/update/ota/ota.tar.gz"

/*!********************************************************************************************************************
 *  STATIC VARIABLES
 *********************************************************************************************************************/
OtaManagerCloudCommunication *OtaManagerCloudCommunication::GetInstance() {
    static OtaManagerCloudCommunication instance;
    return &instance;
}

/*!********************************************************************************************************************
 *  CLASS FUNCTIONS
 *********************************************************************************************************************/

OtaManagerCloudCommunication::OtaManagerCloudCommunication()
    : curl_handle_(nullptr)
    , curl_header_(nullptr)
    , download_url_("")
    , is_need_update_(false)
    , is_upload_result_(false)
    , is_start_download_(false) 
    , is_auth_ok_(false){
    Init();
}

OtaManagerCloudCommunication::~OtaManagerCloudCommunication() {
    DeInit();
}

uint32_t OtaManagerCloudCommunication::Init() {
    curl_header_ = curl_slist_append(curl_header_, "Content-Type:application/json");
    curl_header_ = curl_slist_append(curl_header_, "charsets:utf-8");
    return 0;
}

uint32_t OtaManagerCloudCommunication::CheckTask() {
    is_need_update_ = false;
    std::string res_string;
    std::string string_json = getCheckTaskContent();
    if (string_json.length() == 0) {
        LOG_ERR("string_json length is 0.");
        return 1;
    }
    curl_handle_ = curl_easy_init();
    if (curl_handle_ == nullptr) {
        LOG_ERR("curl_handle_ init failed.");
        return 1;
    }
    LOG_DEBUG("%s", string_json.c_str());
    std::string url = std::string(OTA_CLOUD_URL_HEAD) + std::string("/ota/vehicle/ibox/heart");
    curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str());
    //curl_easy_setopt(curl_handle_, CURLOPT_VERBOSE, 1);
    setCert();
    curl_easy_setopt(curl_handle_, CURLOPT_HTTPHEADER, curl_header_);
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDS, string_json.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDSIZE, string_json.length());
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, (void *)&res_string);
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, CheckTaskCallback);
    curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl_handle_, CURLOPT_CONNECTTIMEOUT, 5);
    LOG_INFO("%s", res_string.c_str());
    CURLcode res = curl_easy_perform(curl_handle_);
    if (res != CURLE_OK) {
        LOG_ERR("CheckTask failed:%d", static_cast<int32_t>(res));
        curl_easy_cleanup(curl_handle_);
        return 1;
    }
    curl_easy_cleanup(curl_handle_);
    return 0;
}

uint32_t OtaManagerCloudCommunication::AuthTask() {
    is_auth_ok_ = false;
    seed_ = getSeed();
    std::string res_string;
    json json_data;
    json_data["vehicleVin"] = getDeviceID();
    json_data["seed"] = seed_;
    std::string string_json = json_data.dump();
    curl_handle_ = curl_easy_init();
    if (curl_handle_ == nullptr) {
        LOG_ERR("curl_handle_ init failed.");
        return 1;
    }
    std::string url = std::string(OTA_CLOUD_URL_HEAD) + std::string("/ota/vehicle/ibox/authentication");
    LOG_ERR("%s", url.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str());
    setCert();
    curl_easy_setopt(curl_handle_, CURLOPT_HTTPHEADER, curl_header_);
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDS, string_json.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDSIZE, string_json.length());
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, (void *)&res_string);
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, AuthCallback);
    curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl_handle_, CURLOPT_CONNECTTIMEOUT, 5);
    CURLcode res = curl_easy_perform(curl_handle_);
    if (res != CURLE_OK) {
        LOG_ERR("AuthTask failed:%d", static_cast<int32_t>(res));
        curl_easy_cleanup(curl_handle_);
        return 1;
    }
    curl_easy_cleanup(curl_handle_);
    return 0;
}

uint32_t OtaManagerCloudCommunication::ConfirmTask() {
    is_start_download_ = false;
    std::string res_string;
    json json_data;
    json_data["vehicleVin"] = getDeviceID();
    json_data["AuthenticationResult"] = 0;
    std::string string_json = json_data.dump();
    if (string_json.length() == 0) {
        LOG_ERR("string_json length is 0.");
    }
    curl_handle_ = curl_easy_init();
    if (curl_handle_ == nullptr) {
        LOG_ERR("curl_handle_ init failed.");
        return 1;
    }
    std::string url = std::string(OTA_CLOUD_URL_HEAD) + std::string("/ota/vehicle/ibox/download");
    setCert();
    curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_HTTPHEADER, curl_header_);
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDS, string_json.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDSIZE, string_json.length());
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, (void *)&res_string);
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, ConfirmCallback);
    curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl_handle_, CURLOPT_CONNECTTIMEOUT, 5);
    CURLcode res = curl_easy_perform(curl_handle_);
    if (res != CURLE_OK) {
        LOG_ERR("ConfirmTask failed");
        curl_easy_cleanup(curl_handle_);
        return 1;
    }
    curl_easy_cleanup(curl_handle_);
    return 0;
}

uint32_t OtaManagerCloudCommunication::DownloadFile() {
    FILE *ota_file = fopen(OTA_FILE_NAME, "wb");//如果使用断点续传， 应该更改打开方式为ab， 目前不建议断点续传
    if (ota_file == nullptr) {
        LOG_ERR("open ota_file  failed.");
        return 1;
    }
    fseek(ota_file, 0, SEEK_END);
    int32_t file_size = ftell(ota_file);
    curl_handle_ = curl_easy_init();
    if (curl_handle_ == nullptr) {
        LOG_ERR("curl_handle_ init failed.");
        fclose(ota_file);
        return 1;
    }
    setCert();
    curl_easy_setopt(curl_handle_, CURLOPT_URL, download_url_.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, (void *)ota_file);
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, DownloadFileCallback);
    //curl_easy_setopt(curl_handle_, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl_handle_, CURLOPT_RESUME_FROM, file_size);//resume
    // download file
    curl_easy_setopt(curl_handle_, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl_handle_, CURLOPT_PROGRESSDATA, DownloadProgressCallback);
    curl_easy_setopt(curl_handle_, CURLOPT_CONNECTTIMEOUT, 10);
    CURLcode res = curl_easy_perform(curl_handle_);
    if (res != CURLE_OK) {
        fclose(ota_file);
        curl_easy_cleanup(curl_handle_);
        LOG_ERR("DownloadFile failed");
        return 1;
    }
    fclose(ota_file);
    curl_easy_cleanup(curl_handle_);
    return 0;
}

uint32_t OtaManagerCloudCommunication::UploadProgress(int32_t progress) {
    std::string res_string;
    json json_data;
    json_data["vehicleVin"] = getDeviceID();
    json_data["upgradeProgress"] = progress;
    std::string string_json = json_data.dump();
    if (string_json.length() == 0) {
        LOG_ERR("string_json length is 0.");
        return 1;
    }
    curl_handle_ = curl_easy_init();
    if (curl_handle_ == nullptr) {
        LOG_ERR("curl_handle_ init failed.");
        return 1;
    }
    std::string url = std::string(OTA_CLOUD_URL_HEAD) + std::string("/ota/vehicle/ibox/progress");
    setCert();
    curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_HTTPHEADER, curl_header_);
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDS, string_json.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDSIZE, string_json.length());
    curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl_handle_, CURLOPT_CONNECTTIMEOUT, 5);
    CURLcode res = curl_easy_perform(curl_handle_);
    if (res != CURLE_OK) {
        LOG_ERR("UploadProgress failed:%d", static_cast<int32_t>(res));
        curl_easy_cleanup(curl_handle_);
        return 1;
    }
    curl_easy_cleanup(curl_handle_);
    return 0;
}

uint32_t OtaManagerCloudCommunication::UploadResult() {
    is_upload_result_ = false;
    std::string res_string;
    std::string string_json = getUploadResultContent();
    if (string_json.length() == 0) {
        LOG_ERR("string_json length is 0.");
        return 1;
    }
    curl_handle_ = curl_easy_init();
    if (curl_handle_ == nullptr) {
        LOG_ERR("curl_handle_ init failed.");
        return 1;
    }
    std::string url = std::string(OTA_CLOUD_URL_HEAD) + std::string("/ota/vehicle/ibox/result");
    setCert();
    curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_HTTPHEADER, curl_header_);
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDS, string_json.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDSIZE, string_json.length());
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, (void *)&res_string);
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, UploadResultCallback);
    curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl_handle_, CURLOPT_CONNECTTIMEOUT, 5);
    CURLcode res = curl_easy_perform(curl_handle_);
    if (res != CURLE_OK) {
        LOG_ERR("UploadResult failed");
        curl_easy_cleanup(curl_handle_);
        return 1;
    }
    curl_easy_cleanup(curl_handle_);
    return 0;
}

uint32_t OtaManagerCloudCommunication::DeInit() {
    curl_easy_cleanup(curl_handle_);
    curl_slist_free_all(curl_header_);
    return 0;
}

std::string OtaManagerCloudCommunication::getCheckTaskContent() {
    json json_data;
    json_data["vehicleVin"] = getDeviceID();
    json_data["SoftwareVersion"] = getVersion();
    return json_data.dump();
}

std::string OtaManagerCloudCommunication::getDownloadFileContent() {}

std::string OtaManagerCloudCommunication::getUploadResultContent() {
    json json_data;
    json_data["vehicleVin"] = getDeviceID();
    json_data["upgradeResult"] = OtaManagerProcessStorage::GetInstance()->GetResultCode();
    return json_data.dump();
}

std::string OtaManagerCloudCommunication::getVersion() {
    char version[64] = {0,};
     FILE * file_version = fopen(version_path_.c_str(), "rb");
    if (file_version != nullptr) {
        if (fread(version, sizeof(char), 64, file_version) > 0) {
            fclose(file_version);
            return std::string(version);
        }
        fclose(file_version);
    } else {
        LOG_ERR("open file :%s failed", version_path_.c_str());
    }
    return std::string("v0.0.0");
}

std::string OtaManagerCloudCommunication::getDeviceID() {
    std::ifstream file_stream;
    std::string device_id;
    file_stream.clear();
    file_stream.open(device_id_path_.c_str(), std::ifstream::in);
    if (file_stream.is_open()) {
        nlohmann::json cjson;
        file_stream >> cjson;
        if (cjson.contains("DeviceId")) {
            cjson.at("DeviceId").get_to(device_id);
        }
        file_stream.close();
        return device_id;
    }
    return std::string("no_device_id");
}

std::string OtaManagerCloudCommunication::getSeed() {
        BIGNUM *bn;
        bn = BN_new(); //生成一个BIGNUM结构
        int bits = 256;
        int top = -1;
        int bottom = 1;
        BN_rand(bn, bits, top, bottom); //生成指定bits的随机数
        char *hex = BN_bn2hex(bn); //转化成16进制字符串
        BN_free(bn); //释放BIGNUM结构
        return std::string(hex);
}

bool OtaManagerCloudCommunication::verifySeed(std::string encrypted_seed) {
    //解码base
    EVP_ENCODE_CTX  eenctx;
    char sig[344] = {0};
    int tlen = 0;
      int signLen =  0;
    EVP_DecodeInit(&eenctx);
    if (EVP_DecodeUpdate(&eenctx, (unsigned char*)sig, &signLen, (unsigned char*)encrypted_seed.c_str(), encrypted_seed.length()) < 0){
                    printf("EVP_DecodeUpdate is failed.\n");
        return false;
    }
    tlen+=signLen;

    if (EVP_DecodeFinal(&eenctx, (unsigned char*)sig+tlen, &signLen) < 0){
                printf("EVP_DecodeFinal is failed.\n");
    return false;
    }
    tlen+=signLen;

    //读取rsa公钥
    RSA *rsa_pub = RSA_new();

    FILE * file_pub = fopen(public_key_path_.c_str(), "rb");
    if (file_pub == nullptr) {
        if (rsa_pub != nullptr) {
            RSA_free(rsa_pub);
        }
        return false;
    }
    if ((rsa_pub = PEM_read_RSA_PUBKEY(file_pub, &rsa_pub, NULL, NULL)) == NULL){
        LOG_ERR("PEM_read_RSAPublickey failed");
        if (rsa_pub != nullptr) {
            RSA_free(rsa_pub);
        }
        fclose(file_pub);
        return false;
    }
    fclose(file_pub);
    int32_t len = RSA_size(rsa_pub);
    char *decrypted_seed = (char *)malloc(len + 1);
    memset(decrypted_seed, 0, len + 1);
    //解密
    int ret = RSA_public_decrypt(tlen, (const unsigned char*)sig, (unsigned char*)decrypted_seed, rsa_pub, RSA_PKCS1_PADDING);
    if (ret >= 0) {
        
        std::string seed = std::string(decrypted_seed, ret);
        if (strcmp(seed_.c_str(), seed.c_str()) == 0) {

            if (rsa_pub != nullptr) {
                RSA_free(rsa_pub);
            }
            if  (decrypted_seed != nullptr) {
                free(decrypted_seed);
            }
            return true;
        }
    }
    if (rsa_pub != nullptr) {
        RSA_free(rsa_pub);
    }
    if  (decrypted_seed != nullptr) {
        free(decrypted_seed);
    }
    return false;
}

void OtaManagerCloudCommunication::setCert()
{
    curl_easy_setopt(curl_handle_, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl_handle_, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl_handle_, CURLOPT_CAINFO, cert_root_path_.c_str());
    // curl_easy_setopt(curl_handle_, CURLOPT_SSL_VERIFYHOST, 2L);
    // curl_easy_setopt(curl_handle_, CURLOPT_SSLCERT, cert_client_path_.c_str());
    // curl_easy_setopt(curl_handle_, CURLOPT_SSLCERTTYPE, "pem");
    // curl_easy_setopt(curl_handle_, CURLOPT_SSLKEY, cert_key_path_.c_str());
    // curl_easy_setopt(curl_handle_, CURLOPT_SSLKEYTYPE, "pem");
}

size_t OtaManagerCloudCommunication::CheckTaskCallback(void *buffer, size_t size, size_t nmemb, void *userp) {
    if (userp == nullptr || buffer == nullptr || size == 0) {
        return 0;
    }
    uint32_t real_size = size * nmemb;
    std::string *pstr = (std::string *)userp;

    if (pstr != nullptr) {
        pstr->append((const char *)buffer, real_size);
        LOG_DEBUG("%s", pstr->c_str());
        if (json::accept(pstr->c_str())) {
            json reps_json = json::parse(pstr->c_str());

            if (reps_json["code"] == 0 && reps_json["data"] == 0) {
                OtaManagerCloudCommunication::GetInstance()->is_need_update_ = true;
            }
        }
    }
    return size * nmemb;
}

size_t OtaManagerCloudCommunication::AuthCallback(void *buffer, size_t size, size_t nmemb, void *userp) {
    if (userp == nullptr || buffer == nullptr || size == 0) {
        return 0;
    }
    uint32_t real_size = size * nmemb;
    std::string *pstr = (std::string *)userp;
    if (pstr != nullptr) {
        pstr->append((const char *)buffer, real_size);
        LOG_DEBUG("%s", pstr->c_str());
        if (json::accept(pstr->c_str())) {
            json reps_json = json::parse(pstr->c_str());
            if (reps_json["code"] == 0) {
                std::string key_value = reps_json["data"];
                //todo check key value
                if (OtaManagerCloudCommunication::GetInstance()->verifySeed(key_value)) {
                    LOG_INFO("verifySeed ok");
                    OtaManagerCloudCommunication::GetInstance()->is_auth_ok_ = true;
                }
            }
        }
    }
    return real_size;
}

size_t OtaManagerCloudCommunication::ConfirmCallback(void *buffer, size_t size, size_t nmemb, void *userp) {
    if (userp == nullptr || buffer == nullptr || size == 0) {
        return 0;
    }
    uint32_t real_size = size * nmemb;
    std::string *pstr = (std::string *)userp;
    if (pstr != nullptr) {
        pstr->append((const char *)buffer, real_size);
        LOG_DEBUG("%s", pstr->c_str());

        if (json::accept(pstr->c_str())) {
            json reps_json = json::parse(pstr->c_str());
            if (reps_json["code"] == 0) {
                OtaManagerCloudCommunication::GetInstance()->download_url_ = reps_json["data"];
                // todo  save download url to disk

                OtaManagerCloudCommunication::GetInstance()->is_start_download_ = true;
            }
        }
    }
    return real_size;
}

size_t OtaManagerCloudCommunication::DownloadFileCallback(void *buffer, size_t size, size_t nmemb, void *userp) {
    LOG_DEBUG("size:%d==nmemb==%d", size, nmemb);
    if (userp == nullptr || buffer == nullptr || size == 0) {
        return 0;
    }
    uint32_t real_size = size * nmemb;
    LOG_DEBUG("userp:%p, real_size:%d", userp,  real_size);
    size_t dataByte = fwrite(buffer, size, nmemb, (FILE *)userp);
    LOG_DEBUG("dataByte:%d",  dataByte);
    return dataByte;
}

size_t OtaManagerCloudCommunication::DownloadProgressCallback(void *data, double dltotal, double dlnow, double ultotal, double ulnow) {
    if (dltotal > 1) {
        double_t progress = dlnow * 100.0 / dltotal;
        std::cout << "download progress: " << progress << "%" << std::endl;
    }
}

size_t OtaManagerCloudCommunication::UploadResultCallback(void *buffer, size_t size, size_t nmemb, void *userp) {
    if (userp == nullptr || buffer == nullptr || size == 0) {
        return 0;
    }
    uint32_t real_size = size * nmemb;
    std::string *pstr = (std::string *)userp;
    if (pstr != nullptr) {
        pstr->append((const char *)buffer, real_size);
        if (json::accept(pstr->c_str())) {
            json reps_json = json::parse(pstr->c_str());
            if (reps_json["code"] == 0) {
                OtaManagerCloudCommunication::GetInstance()->is_upload_result_ = true;
            }
        }
    }
    return size * nmemb;
}

bool OtaManagerCloudCommunication::IsNeedUpdate() { return is_need_update_; }

bool OtaManagerCloudCommunication::IsSuccessUploadResult() { return is_upload_result_; }

bool OtaManagerCloudCommunication::IsStartDownload() { return is_start_download_; }

bool OtaManagerCloudCommunication::IsAuthOkay() { return is_auth_ok_; }

