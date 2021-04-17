#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"
#include "spdlog/spdlog.h"

class FLogManager : TSingleton<FLogManager>
{
public:
    FLogManager();
    ~FLogManager();

    void LogMessage();

    void Init();
    void UnInit();

private:
    spdlog::logger* mLogFile;
    std::string mLogFileName;
};
