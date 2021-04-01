#include "FBuffer.h"
#include "FDeviceManager.h"
#include "FDevice.h"


FBuffer::FBuffer()
{
}

FBuffer::~FBuffer()
{
}

FVertexBuffer::FVertexBuffer()
{
}

FVertexBuffer::~FVertexBuffer()
{
}

void FVertexBuffer::Init()
{
    TSingleton<FDeviceManager>::GetInstance().GetRootDevice()->CreateVertexBuffer(this);
}

void FVertexBuffer::Uninit()
{
}


FIndexBuffer::FIndexBuffer()
{
}

FIndexBuffer::~FIndexBuffer()
{
}

void FIndexBuffer::Init()
{
    TSingleton<FDeviceManager>::GetInstance().GetRootDevice()->CreateIndexBuffer(this);
}

void FIndexBuffer::Uninit()
{
}

