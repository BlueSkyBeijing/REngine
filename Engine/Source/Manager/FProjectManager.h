#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"


class FProjectManager
{
    friend class TSingleton<FProjectManager>;

public:
    void Init();
    void UnInit();

    bool LoadProjectFile(const FString& ProjectFile);

private:
    FProjectManager();
    virtual ~FProjectManager();

private:
};
