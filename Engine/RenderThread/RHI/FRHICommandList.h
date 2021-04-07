#pragma once
#include "Prerequisite.h"

class FRHICommandList
{
public:
	FRHICommandList();
	~FRHICommandList();

private:

};

template<typename ReturnType = void>
struct FRenderCommand
{
public:
    template<class Function, class... Args>
    void Wrap(Function&& function, Args && ... args)
    {
        mFunction = [&] {return function(args...); };
    }

    ReturnType Excecute()
    {
        return mFunction();
    }

private:
    std::function < ReturnType()> mFunction;
};