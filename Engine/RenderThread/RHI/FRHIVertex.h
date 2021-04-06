#pragma once
#include "Prerequisite.h"


class FRHIVertex
{
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

class FStaticMeshVertex : public FRHIVertex
{
public:
	FStaticMeshVertex();
	~FStaticMeshVertex();
	
public:
	FVector3 Position;
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
