#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"


class FModuleManager
{
    friend class TSingleton<FModuleManager>;

public:
    void Init();
    void UnInit();

    IModuleInterface* GetOrCreate(const FString& InModuleName);

    bool LoadProjectMoudules(const FString& projectPath, const FString& projectName);

private:
    FModuleManager();
    virtual ~FModuleManager();

    IModuleInterface* FindModule(FString InModuleName);

private:
    TMap<FString, IModuleInterface*> Modules;

};
