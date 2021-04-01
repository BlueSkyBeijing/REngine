#pragma once


class FRHIResource
{
public:
	FRHIResource();
	~FRHIResource();

    virtual void Init() {}
    virtual void UnInit() {}

private:

};
