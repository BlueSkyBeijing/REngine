#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FLogManager : TSingleton<FLogManager>
{
public:
    FLogManager();
    ~FLogManager();

    void Init();
    void UnInit();

private:

};
