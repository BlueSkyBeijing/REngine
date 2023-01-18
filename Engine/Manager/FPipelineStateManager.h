#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"


class FPipelineStateManager
{
    friend class TSingleton<FPipelineStateManager>;

public:
    void Init();
    void UnInit();

    FRHIPipelineState* CreatePipleLineState(const FPipelineStateInfo& info);

    FRHIPipelineState* GetPipleLineState(const FPipelineStateInfo& info);

private:
    FPipelineStateManager();
    virtual ~FPipelineStateManager();

private:
    TMap<uint64, FRHIPipelineState*> mPipelineStates;
};
