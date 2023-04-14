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

#include "ota_manager_common.h"
#include "ota_manager_state.h"
#include "CUtil.h"


OtaManagerState::OtaManagerState()
{

}

 OtaManagerState::~OtaManagerState()
{

}


uint32_t  OtaManagerState::Init()
{
    LOG_INFO("OtaManagerState Init");
    return 0;
}
uint32_t  OtaManagerState::DeInit()
{
    LOG_INFO("OtaManagerState DeInit");
    return 0;
}

uint32_t  OtaManagerState::ParseAndVerify()
{
    LOG_INFO("OtaManagerState ParseAndVerify");
    return 0;
}

uint32_t  OtaManagerState::PrepareDownload()
{
    LOG_INFO("OtaManagerState PrepareDownload");
    return 0;
}

uint32_t  OtaManagerState::Update()
{
     LOG_INFO("OtaManagerState Update");
    return 0;   
}

uint32_t  OtaManagerState::Download()
{
     LOG_INFO("OtaManagerState Download");
    return 0;   
}

uint32_t  OtaManagerState::CheckResult()
{
     LOG_INFO("OtaManagerState CheckResult");
    return 0;   
}

uint32_t  OtaManagerState::NotifyResult()
{
     LOG_INFO("OtaManagerState NotifyResult");
    return 0;   
}