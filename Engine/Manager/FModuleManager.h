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

private:
    FModuleManager();
    virtual ~FModuleManager();

private:
};
