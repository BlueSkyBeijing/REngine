#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FSceneManager : TSingleton<FSceneManager>
{
public:
	FSceneManager();
	~FSceneManager();

    void Init();
    void UnInit();

private:

};
