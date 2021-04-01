#pragma once


class FRenderResource
{
public:
	FRenderResource();
	~FRenderResource();

    virtual void Init() {}
    virtual void UnInit() {}

private:

};
