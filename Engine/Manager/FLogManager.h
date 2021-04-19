#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

enum ELogLevel
{
    LL_Debug,
    LL_Info,
    LL_Warning,
    LL_Error,
};

class FLogManager : TSingleton<FLogManager>
{
public:
    FLogManager();
    ~FLogManager();

    void LogMessage(ELogLevel level, const char* loginfo);

    void Init();
    void UnInit();

private:
    spdlog::logger* mLogFile;
    std::string mLogFileName;
};
