#include <csignal>
#include <iostream>

#include "ota_manager.h"
#include "CUtil.h"

using namespace std;

 std::unique_ptr<IBOXOTAManager> gIBOXOTAManager = nullptr;

/** Signal handler.*/
static void SigHandler(int signum)
{
    LOG_WARN("Received signal: %u. Quitting\n", signum);
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    gIBOXOTAManager->Stop();
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGHUP, SIG_DFL);
}

/** Sets up signal handler.*/
static void SigSetup(void)
{
    struct sigaction action{};
    action.sa_handler = SigHandler;

    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGHUP, &action, NULL);
}

int main(void)
{
   gIBOXOTAManager.reset(new IBOXOTAManager());
   gIBOXOTAManager->Init();
   gIBOXOTAManager->Run();
    return 0;
}
