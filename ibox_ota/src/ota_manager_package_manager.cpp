/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <iostream>
#include <memory>

#include "json.hpp"
#include "CUtil.h"
#include "ota_manager_common.h"
#include "ota_manager_package_manager.h"

OtaManagerPackageManager *OtaManagerPackageManager::GetInstance()
{
    static OtaManagerPackageManager instance;
    return &instance;
}


OtaManagerPackageManager::OtaManagerPackageManager()
{

}

 OtaManagerPackageManager::~OtaManagerPackageManager()
{

}


std::string OtaManagerPackageManager::GetTopDirectory()
{
    std::lock_guard<std::mutex> lock(mTopDirMutex);
    return mTopDirectory;
}

uint32_t OtaManagerPackageManager::SetTopDirectory(std::string topDir)
{
    std::lock_guard<std::mutex> lock(mTopDirMutex);
    mTopDirectory = topDir;
    return 0;
}

uint32_t OtaManagerPackageManager::AddWaitingPackageInfo(PackageInfo packInfo)
{
    std::lock_guard<std::mutex> lock(mPackageMutex);
    mWaitingPackageList.emplace_back(packInfo);
    //todo add json;
    return 0;
}
uint32_t OtaManagerPackageManager::GetWaitingPackageInfoSize()
{
    std::lock_guard<std::mutex> lock(mPackageMutex);
    return mWaitingPackageList.size();
}

uint32_t OtaManagerPackageManager::GetWaitingPackageInfo(PackageInfo *packInfo)
{
    std::lock_guard<std::mutex> lock(mPackageMutex);
    if (mWaitingPackageList.empty() != false) {
        *packInfo = mWaitingPackageList.front();
        mWaitingPackageList.erase(mWaitingPackageList.begin());
    } else {
        LOG_WARN("mWaitingPackageList is empty");
        return -1;
    }
    return  0;
}
