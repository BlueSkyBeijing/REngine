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
