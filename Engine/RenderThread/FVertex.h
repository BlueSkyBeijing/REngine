#pragma once

#include "Utility.h"
#include <vector>
#include <d3d12.h>

class FVertex
{
};

class FVertexLayout
{
public:
	FVertexLayout();
	~FVertexLayout();

public:
	std::vector<D3D12_INPUT_ELEMENT_DESC> Elements;

private:

};

class FStaticMeshVertex : public FVertex
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
