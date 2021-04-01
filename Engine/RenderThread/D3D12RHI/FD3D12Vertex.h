#pragma once

#include "Utility.h"
#include <vector>
#include <d3d12.h>

class FD3D12Vertex
{
};

class FD3D12VertexLayout
{
public:
	FD3D12VertexLayout();
	~FD3D12VertexLayout();

public:
	std::vector<D3D12_INPUT_ELEMENT_DESC> Elements;

private:

};

class FStaticMeshVertex : public FD3D12Vertex
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
