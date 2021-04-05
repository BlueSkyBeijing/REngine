#include "PrecompiledHeader.h"
#include <windows.h>

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#include "FEngine.h"
#include "TSingleton.h"

int main()
{
    //enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    TSingleton<FEngine>::GetInstance().Launch();

    return 0;
}
