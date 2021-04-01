#pragma once
#include "TSingleton.h"
#include <wrl/client.h>
#include "d3d12.h"

class FPipelineState;
class FRenderProxy;

class FPipelineStateManager : TSingleton<FPipelineStateManager>
{
public:
	FPipelineStateManager();
	~FPipelineStateManager();

    void Init();
    void UnInit();

    FPipelineState* GetOrCreatePipleLineState(FRenderProxy* renderProxy);

private:
	FPipelineState* mPipelineState;
};
