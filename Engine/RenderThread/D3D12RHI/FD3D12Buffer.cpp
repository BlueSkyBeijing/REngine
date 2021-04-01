#include "FD3D12Buffer.h"
#include "FD3D12RHIManager.h"
#include "FD3D12RHI.h"


FD3D12Buffer::FD3D12Buffer()
{
}

FD3D12Buffer::~FD3D12Buffer()
{
}

FD3D12VertexBuffer::FD3D12VertexBuffer()
{
}

FD3D12VertexBuffer::~FD3D12VertexBuffer()
{
}

void FD3D12VertexBuffer::Init()
{
    TSingleton<FD3D12RHIManager>::GetInstance().GetRootDevice()->CreateVertexBuffer(this);
}

void FD3D12VertexBuffer::Uninit()
{
}


FD3D12IndexBuffer::FD3D12IndexBuffer()
{
}

FD3D12IndexBuffer::~FD3D12IndexBuffer()
{
}

void FD3D12IndexBuffer::Init()
{
    TSingleton<FD3D12RHIManager>::GetInstance().GetRootDevice()->CreateIndexBuffer(this);
}

void FD3D12IndexBuffer::Uninit()
{
}

