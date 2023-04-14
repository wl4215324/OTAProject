/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#ifndef _OTA_MANAGER_PROCESS_STORAGE_H
#define _OTA_MANAGER_PROCESS_STORAGE_H

#include <cstring>
#include <list>
#include <mutex>
#include <sys/mman.h>
#include <thread>  // std::thread
#include <map>
#include <vector>
#include <mutex>
#include <iostream>
#include <fstream>
#include "ota_manager_common.h"
#include "ota_manager_process_storage.h"
#include "json.hpp"

struct ProcessorsState {
    OtaManagerMainState main_state;  
    OtaManagerSubState sub_state;    
    OtaUpdateResultCode result_code; 
};

class OtaManagerProcessStorage{
public:
    static OtaManagerProcessStorage *GetInstance();
    ~OtaManagerProcessStorage();
    OtaManagerSubState GetSubState();
    void SetSubState(const OtaManagerSubState &subState);
    OtaManagerMainState GetMainState();
    void SetMainState(const OtaManagerMainState &mainState);
    OtaUpdateResultCode GetResultCode();
    void SetResultCode(const OtaUpdateResultCode &resultState);    
    void Clear();
    bool LoadProgressorsDb();
private:
    OtaManagerProcessStorage();
    bool UpdateProgressorsDb();

    std::mutex ota_mutex_;
    ProcessorsState processor_state_;
    std::string sw_progressors_db_name_{"sw_processors_db.json"}; // 存放主/子状态 等
};

#endif//_OTA_MANAGER_PROCESS_STORAGE_H
