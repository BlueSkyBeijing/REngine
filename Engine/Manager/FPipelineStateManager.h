#pragma once
#include "TSingleton.h"
#include <wrl/client.h>
#include "d3d12.h"

class FRHIPipelineState;
class FRenderProxy;

class FPipelineStateManager : TSingleton<FPipelineStateManager>
{
public:
	FPipelineStateManager();
	~FPipelineStateManager();

    void Init();
    void UnInit();

    FRHIPipelineState* GetOrCreatePipleLineState(FRenderProxy* renderProxy);

private:
	FRHIPipelineState* mPipelineState;
};
