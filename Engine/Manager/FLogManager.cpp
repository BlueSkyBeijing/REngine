#include "PrecompiledHeader.h"

#include "FLogManager.h"

FLogManager::FLogManager() :
    mLogFileName("Saved\\Logs\\Engine.log")
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

void FLogManager::LogMessage(const char* loginfo)
{
    mLogFile->info(loginfo);
}
