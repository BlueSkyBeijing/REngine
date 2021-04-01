#pragma once

#include "Utility.h"
#include <vector>
#include <d3d12.h>

class FRHIVertex
{
};

class FRHIVertexLayout
{
public:
	FRHIVertexLayout();
	~FRHIVertexLayout();

public:

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
