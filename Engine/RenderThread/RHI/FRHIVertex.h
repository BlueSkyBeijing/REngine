#pragma once
#include "Prerequisite.h"


class FRHIVertex
{
public:
    FRHIVertex();
    FRHIVertex(FVector3 pos);
    ~FRHIVertex();
public:
    FVector3 Position;

};

class FRHIVertexLayout
{
public:
    FRHIVertexLayout();
    ~FRHIVertexLayout();

public:
    std::vector<FInputElementDesc> Elements;
private:

};

class FScreenVertex : public FRHIVertex
{
public:
    FScreenVertex();
    FScreenVertex(FVector3 pos, FVector2 uv);
    ~FScreenVertex();

public:
    FVector2 TexCoord;
private:

};

class FStaticMeshVertex : public FRHIVertex
{
public:
    FStaticMeshVertex();
    ~FStaticMeshVertex();

public:
    FVector3 Normal;
    FVector2 TexCoord;
private:

};

enum EInputClassification
{
    ICF_PER_VERTEX_DATA = 0,
    ICF_PER_INSTANCE_DATA = 1
};

struct FInputElementDesc
{
    LPCSTR SemanticName;
    uint32 SemanticIndex;
    EPixelFormat Format;
    uint32 InputSlot;
    uint32 AlignedByteOffset;
    EInputClassification InputSlotClass;
    uint32 InstanceDataStepRate;
};
