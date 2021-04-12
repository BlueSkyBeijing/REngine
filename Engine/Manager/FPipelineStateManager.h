#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FPipelineStateManager : TSingleton<FPipelineStateManager>
{
public:
    FPipelineStateManager();
    ~FPipelineStateManager();

    void Init();
    void UnInit();

    FRHIPipelineState* CreatePipleLineState(FRenderProxy* renderProxy);
    FRHIPipelineState* GetPipleLineState(FRenderProxy* renderProxy);

private:
    FRHIPipelineState* mPipelineState;
};
