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
    FVector2 UV;
private:

};

class FStaticMeshVertex : public FRHIVertex
{
public:
    FStaticMeshVertex();
    ~FStaticMeshVertex();

public:
    FVector3 Normal;
    FVector2 UV;
private:

};

class FSkeletalMeshVertex : public FRHIVertex
{
public:
    FSkeletalMeshVertex();
    ~FSkeletalMeshVertex();

public:
    FVector3 Normal;
    FVector2 UV;
    FVector4 BoneWeights;
    FVector2 BoneIndices;
private:

};

enum EInputClassification
{
    ICF_PerVertexData = 0,
    ICF_PerInstanceData = 1
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
