#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FLogManager : TSingleton<FLogManager>
{
public:
    FLogManager();
    ~FLogManager();

    void LogMessage(const char* loginfo);

    void Init();
    void UnInit();

private:
    spdlog::logger* mLogFile;
    std::string mLogFileName;
};
