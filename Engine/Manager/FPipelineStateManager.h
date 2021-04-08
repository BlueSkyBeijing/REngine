#pragma once
#include "TSingleton.h"

class FRHIPipelineState;
class FRenderProxy;

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
