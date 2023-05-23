#include "vld.h"
#include "PrecompiledHeader.h"

#include "FEngine.h"
#include "TSingleton.h"
#include "FConsoleVariableManager.h"
#include "FConfigManager.h"
#include "Utility.h"

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

LRESULT CALLBACK ConsoleVariableProc(int code, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;

    long ret = 0;
    if ((WM_KEYUP == wParam) && (kbd->vkCode == VK_TAB))
    {
        TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand();
        ret = 1;
    }
    return ret;
}

int main(int argc, char* argv[])
{
    if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CosonleHandler, TRUE) == FALSE)
    {
        printf("unable to install handler!");
        return -1;
    }

    if (argc > 1)
    {
        FString filePath;
        FString fileName;
        FString name;
        FString suffix;
        FullFileNameSplit(argv[1], filePath, fileName, name, suffix);
        *FConfigManager::ProjectDir = filePath;
        *FConfigManager::ProjectName = name;
    }

    if (argc > 3)
    {
        *FConfigManager::Platform = argv[2];
        *FConfigManager::Configuration = argv[3];
    }

    SetWindowsHookEx(WH_KEYBOARD_LL, ConsoleVariableProc, NULL, 0);

    TSingleton<FEngine>::GetInstance().Launch();

    //when close console window notify handler
    std::unique_lock<std::mutex> ExitLock(EngineMutex);
    EngineExited.store(true);
    EngineExitCondition.notify_one();

    return 0;
}
