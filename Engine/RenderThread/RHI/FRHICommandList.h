#pragma once
#include "Prerequisite.h"

class FRHICommandList
{
public:
    FRHICommandList();
    ~FRHICommandList();

private:

};

struct FRenderCommand
{
public:
    template<class Function, class... Args>
    void Wrap(Function&& function, Args && ... args)
    {
        mFunction = [&] {return function(args...); };
    }

    void Excecute()
    {
        return mFunction();
    }

private:
    std::function<void()> mFunction;
};
