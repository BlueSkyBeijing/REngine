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
    FRHIPipelineState* CreatePipleLineState(FRHIShader* vertexShader, FRHIShader* pixelShader, FRHIVertexLayout* vertexLayout);
    FRHIPipelineState* GetPipleLineState(FRenderProxy* renderProxy);
    FRHIPipelineState* GetPipleLineStateShadow(FRenderProxy* renderProxy);
    FRHIPipelineState* GetPipleLineStateFullscreenQuad();

private:
    FRHIPipelineState* mPipelineState;
    FRHIPipelineState* mPipelineStateShadow;
    FRHIPipelineState* mPipelineStateFullscreenQuad;

};
