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

protected:
    uint64 hashPipelineState(const void* Data, int32 NumBytes);

private:
    FPipelineStateManager();
    virtual ~FPipelineStateManager();

private:
    std::map<uint64, FRHIPipelineState*> mPipelineStates;
};
