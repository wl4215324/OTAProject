/**
 * @Author: ningshaohui
 * @Date: 2023-02-09 18:21:51
 * @LastEditTime: 2023-02-09 18:21:06
 * @LastEditors: ningshaohui
 * @Description:
 * @
 */

#include "ota_manager.h"
#include <queue>
#include <regex>
#include "ota_manager_event_manager.h"
#include "ota_manager_state.h"
#include "ota_manager_state_context.h"
#include "ota_manager_common.h"
#include "ota_manager_process_storage.h"
#include "CUtil.h"

std::queue<OtaManagerEventID> IBOXOTAManager::received_event_que_;
std::mutex IBOXOTAManager::event_mutex_;

IBOXOTAManager::IBOXOTAManager()
:exit_(false)
{
}


IBOXOTAManager::~IBOXOTAManager()
{

}

void IBOXOTAManager::Init()
{
    std::vector<OtaManagerEventID>otaManagerEventID; 
    for (int i = static_cast<int>(OtaManagerEventID::OTA_MANAGER_EVENT_NONE) + 1; i <=  static_cast<int>(OtaManagerEventID::OTA_MANAGER_EVENT_UPLOAD_RESULT_SUCCESS); i++){
        otaManagerEventID.push_back(static_cast<OtaManagerEventID>(i));
    }

    OtaManagerEventManager::GetInstance()->RegisterEventCallback(OtaManagerModuleID::OTA_MANAGER_MAIN, otaManagerEventID, IBOXOTAManager::eventCallback);
     OtaManagerProcessStorage::GetInstance()->LoadProgressorsDb();
    const OtaManagerMainState mainState {OtaManagerProcessStorage::GetInstance()->GetMainState()};
    OtaManagerEventManager::GetInstance()->TriggerEvent(OtaManagerEventManager::GetInstance()->FindEventFromState(mainState), nullptr);
}

void IBOXOTAManager::DeInit()
{

}

void IBOXOTAManager::Run(void)
{
    OtaManagerEventID currecntEventId = OtaManagerEventID::OTA_MANAGER_EVENT_NONE;
    OtaManagerState *managerSateHandle = nullptr;
    while (!exit_)
    {
        if (received_event_que_.empty() == false) {
            {
                std::lock_guard<std::mutex> lock(event_mutex_);
                currecntEventId = received_event_que_.front();
                received_event_que_.pop();
            }
            switch(currecntEventId) {
                case OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_REQUEST:
                {
                    OTAManagerStateContext::GetInstance()->ChangeState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_PREPARE_DOWNLOAD);
                    managerSateHandle = OTAManagerStateContext::GetInstance()->GetStateHandle();
                    managerSateHandle->PrepareDownload(); 
                    break;
                }
                case OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_START:
                {
                    OTAManagerStateContext::GetInstance()->ChangeState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_DOWNLOAD);
                    managerSateHandle = OTAManagerStateContext::GetInstance()->GetStateHandle();
                    managerSateHandle->Download(); 
                    break;
                }
                
                case OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_FAILED:
                {
                    break;           
                }
                case OtaManagerEventID::OTA_MANAGER_EVENT_DOWNLOAD_SUCCESS:
                {
                    OTAManagerStateContext::GetInstance()->ChangeState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_PARSE_AND_VERIFY);
                    managerSateHandle = OTAManagerStateContext::GetInstance()->GetStateHandle();
                    managerSateHandle->ParseAndVerify();  
                    break;           
                }
                case OtaManagerEventID::OTA_MANAGER_EVENT_PARSE_VERIFY_SUCCESS:
                {
                    OTAManagerStateContext::GetInstance()->ChangeState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_UPDATE);
                    managerSateHandle = OTAManagerStateContext::GetInstance()->GetStateHandle();
                    managerSateHandle->Update(); 
                    break;
                }
                case OtaManagerEventID::OTA_MANAGER_EVENT_UPDATE_SUCCESS:
                {
                    OTAManagerStateContext::GetInstance()->ChangeState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_CHECK_RESULT);
                    managerSateHandle = OTAManagerStateContext::GetInstance()->GetStateHandle();
                    managerSateHandle->CheckResult(); 
                    break;
                }
                case OtaManagerEventID::OTA_MANAGER_EVENT_PARSE_VERIFY_FAILED:
                case OtaManagerEventID::OTA_MANAGER_EVENT_UPDATE_FAILED:
                case OtaManagerEventID::OTA_MANAGER_EVENT_CHECK_UPDATE_RESULT_SUCCESS:
                {
                    OTAManagerStateContext::GetInstance()->ChangeState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_UPLOAD_RESULT);
                    managerSateHandle = OTAManagerStateContext::GetInstance()->GetStateHandle();
                    managerSateHandle->NotifyResult(); 
                    break;
                }
                case OtaManagerEventID::OTA_MANAGER_EVENT_UPLOAD_RESULT_FAILED:
                {
                    break;
                }
                case OtaManagerEventID::OTA_MANAGER_EVENT_UPLOAD_RESULT_SUCCESS:
                {
                    OTAManagerStateContext::GetInstance()->ChangeState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_IDLE);
                    break; 
                }
                default :
                {
                     OTAManagerStateContext::GetInstance()->ChangeState(OtaManagerMainState::OTA_MANAGER_MAIN_STATE_IDLE);
                    break;                    
                }
            }
        }
        sleep(2);
    }
    
}


void IBOXOTAManager::Stop(void)
{
    exit_ = true;
}

void IBOXOTAManager::eventCallback(OtaManagerEventID eventId, void *data)
{
    std::lock_guard<std::mutex> lock(event_mutex_);
    received_event_que_.emplace(eventId);
}








