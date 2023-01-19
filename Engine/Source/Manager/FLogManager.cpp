#include "PrecompiledHeader.h"

#include "FLogManager.h"
#include "FConfigManager.h"

FLogManager::FLogManager() :
    mLogFileName(FConfigManager::LogFile)
{
}

FLogManager::~FLogManager()
{
}

void FLogManager::Init()
{
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(spdlog::level::trace);
    consoleSink->set_pattern("[REngine] [%^%l%$] %v");

    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(mLogFileName, true);
    fileSink->set_level(spdlog::level::trace);

    mLogFile = new spdlog::logger("multi_sink", { consoleSink, fileSink });
    mLogFile->set_level(spdlog::level::trace);
}

void FLogManager::UnInit()
{
    delete mLogFile;
    mLogFile = nullptr;
}

void FLogManager::LogMessage(ELogLevel level, const char* loginfo)
{
    switch (level)
    {
    case LL_Debug:
        mLogFile->debug(loginfo);
        break;
    case LL_Info:
        mLogFile->info(loginfo);
        break;
    case LL_Warning:
        mLogFile->warn(loginfo);
        break;
    case LL_Error:
        mLogFile->error(loginfo);
        break;
    default:
        break;
    }
}
