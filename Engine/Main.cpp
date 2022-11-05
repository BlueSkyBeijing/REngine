#include "PrecompiledHeader.h"

#if defined(DEBUG) | defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#endif

#include "FEngine.h"
#include "TSingleton.h"

std::mutex EngineMutex;
std::condition_variable EngineExitCondition;
std::atomic_bool EngineExited = false;

//the console ctrl handler run in a thread so needs to wait 
static BOOL CALLBACK CosonleHandler(DWORD event)
{
    BOOL returnValue = FALSE;
    switch (event)
    {
    case CTRL_CLOSE_EVENT:
    case CTRL_C_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    case CTRL_LOGOFF_EVENT:
    {
        TSingleton<FEngine>::GetInstance().Exit();

        std::unique_lock<std::mutex> ExitLock(EngineMutex);
        if (!EngineExited.load())
        {
            EngineExitCondition.wait(ExitLock);
        }

        returnValue = TRUE;
    }
    break;
    default:
        break;
    }
    return returnValue;
}

int main()
{
    //enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(3819);
#endif

    if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CosonleHandler, TRUE) == FALSE)
    {
        printf("unable to install handler!");
        return -1;
    }

    TSingleton<FEngine>::GetInstance().Launch();

    //when close console window notify handler
    std::unique_lock<std::mutex> ExitLock(EngineMutex);
    EngineExited.store(true);
    EngineExitCondition.notify_one();

    return 0;
}
