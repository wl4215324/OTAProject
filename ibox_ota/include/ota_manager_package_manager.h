/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#ifndef _OTA_MANAGER_PACKAGE_MANAGER_H
#define _OTA_MANAGER_PACKAGE_MANAGER_H

#include <cstring>
#include <list>
#include <mutex>
#include <sys/mman.h>
#include <thread>  // std::thread
#include "ota_manager_common.h"
#include "json.hpp"

enum class PackgeUpdateState {
  PACKAGE_UPDATE_NONE,
  PACKAGE_UPDATE_FAIL,
  OTA_UPDATE_SUCCESS
};

enum class PackgeType {
  SOFTWARE,
  FIREWARE,
};


using FileInfo = struct front_info {
  std::string fileName;
  std::string filePath;
  std::string fileInstallPath;
  uint32_t fileSize;
};

using PackageInfo = struct package_info {
  std::string packageName;
  std::string packagePath;
  PackgeType packageType;
  PackgeUpdateState packageState;
  uint8_t isVerify;
  uint32_t packageSize;
  uint32_t filenumber;
  std::vector<FileInfo>  fileInfoVec;
};

class OtaManagerPackageManager{
public:
    static OtaManagerPackageManager *GetInstance();
    ~OtaManagerPackageManager();
    std::string GetTopDirectory();
    uint32_t SetTopDirectory(std::string topDir);
    uint32_t AddWaitingPackageInfo(PackageInfo packInfo);
    uint32_t GetWaitingPackageInfoSize();
    uint32_t GetWaitingPackageInfo(PackageInfo *packInfo);
private:
    OtaManagerPackageManager();

private:
    std::string mTopDirectory;
    std::list<PackageInfo> mUpdatedPackageList;
    std::list<PackageInfo> mWaitingPackageList;
    nlohmann::json mPackageJsonData;
    std::mutex mPackageMutex;
    std::mutex mTopDirMutex;
};

#endif//_OTA_MANAGER_PACKAGE_MANAGER_H
