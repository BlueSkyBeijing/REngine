#pragma once
#include "Prerequisite.h"


class FRHIResource
{
public:
    FRHIResource();
    virtual~FRHIResource();

    virtual void Init() {}
    virtual void UnInit() {}

public:

protected:
    bool getInited()
    {
        return mInited;
    }

    void setInited(bool inited)
    {
        mInited = inited;
    }
private:
    bool mInited;

};
