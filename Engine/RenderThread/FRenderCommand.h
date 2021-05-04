#pragma once
#include "Prerequisite.h"

#include "FRenderThread.h"
#include "FEngine.h"
#include "TSingleton.h"

class FRenderCommand
{
public:
    FRenderCommand() {}
    virtual ~FRenderCommand() {}

    virtual void Excecute() = 0;
};

template<typename Lambda>
class TRenderCommand : public FRenderCommand
{
public:
    TRenderCommand(Lambda&& inLambda) : mLambda(std::forward<Lambda>(inLambda)) {}
    virtual ~TRenderCommand() override {}

public:
    virtual void Excecute() override
    {
        mLambda();
    }

private:
    Lambda mLambda;
};

template<typename Lambda>
void EnqueueRenderCommand(Lambda&& lambda)
{
    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();
    TRenderCommand<Lambda>* cmd = new TRenderCommand<Lambda>(std::forward<Lambda>(lambda));
    renderThread->EnqueueRenderCommand(cmd);
}

#define ENQUEUE_RENDER_COMMAND EnqueueRenderCommand
