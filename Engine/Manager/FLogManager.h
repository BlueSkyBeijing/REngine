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

class FLogManager
{
    friend class TSingleton<FLogManager>;

public:
    void LogMessage(ELogLevel level, const char* loginfo);

    void Init();
    void UnInit();

private:
    FLogManager();
    virtual ~FLogManager();

private:
    spdlog::logger* mLogFile;
    std::string mLogFileName;
};
