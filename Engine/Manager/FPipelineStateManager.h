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

    FRHIPipelineState* CreatePipleLineState(const FPipelineStateInfo& info);

    FRHIPipelineState* GetPipleLineState(const FPipelineStateInfo& info);

protected:
    uint64 hashPipelineState(const void* Data, int32 NumBytes);

private:
    std::map<uint64, FRHIPipelineState*> mPipelineStates;
};
