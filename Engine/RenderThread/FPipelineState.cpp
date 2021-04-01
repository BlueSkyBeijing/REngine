#include "FPipelineState.h"
#include "FDeviceManager.h"
#include "FDevice.h"

FPipelineState::FPipelineState()
{
}

FPipelineState::~FPipelineState()
{
}

void FPipelineState::Init()
{
    TSingleton<FDeviceManager>::GetInstance().GetRootDevice()->CreatePipelineState(this);

}

void FPipelineState::UnInit()
{
}
