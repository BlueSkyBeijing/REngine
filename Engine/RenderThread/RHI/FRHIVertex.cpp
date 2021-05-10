#include "PrecompiledHeader.h"

#include "FRHIVertex.h"


FRHIVertex::FRHIVertex()
{
}

FRHIVertex::FRHIVertex(FVector3 pos) :
    Position(pos)
{
}

FRHIVertex::~FRHIVertex()
{
}

FRHIVertexLayout::FRHIVertexLayout()
{
}

FRHIVertexLayout::~FRHIVertexLayout()
{
}

FScreenVertex::FScreenVertex()
{
}

FScreenVertex::FScreenVertex(FVector3 pos, FVector2 uv) : FRHIVertex(pos),
UV(uv)
{
}

FScreenVertex::~FScreenVertex()
{
}

FStaticMeshVertex::FStaticMeshVertex()
{
}

FStaticMeshVertex::~FStaticMeshVertex()
{
}


FSkeletalMeshVertex::FSkeletalMeshVertex()
{
}

FSkeletalMeshVertex::~FSkeletalMeshVertex()
{
}
