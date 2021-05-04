#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FSceneManager
{
    friend class TSingleton<FSceneManager>;

public:
    FSceneManager();
    virtual ~FSceneManager();

    void Init();
    void UnInit();

private:

};
