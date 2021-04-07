#pragma once
#include "Prerequisite.h"

#include "FRHIResource.h"

class FD3D12Resource : public FRHIResource
{
public:
	FD3D12Resource();
	~FD3D12Resource();

    virtual void Init() {}
    virtual void UnInit() {}

private:

};
