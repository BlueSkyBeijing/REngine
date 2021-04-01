#include "UStaticMesh.h"
#include "Utility.h"

#include <iostream>
#include <fstream>

UStaticMesh::UStaticMesh()
{
    FilePathName = L"Content\\StaticMesh\\SM_Rock.stm";
}

UStaticMesh::~UStaticMesh()
{
}

void UStaticMesh::Load()
{
    FRHIVertexLayout layout;
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 } };

    layout.Elements.push_back(inputLayout[0]);
    layout.Elements.push_back(inputLayout[1]);
    layout.Elements.push_back(inputLayout[2]);

    mVertexLayout = layout;

    std::ifstream staticMeshFile(FilePathName, std::ios::in | std::ios::binary);
    if (!staticMeshFile)
    {
        //print error
        return;
    }

    int32 numVertexes;
    staticMeshFile.read((char*)&numVertexes, sizeof(int32));
    mVertexes.resize(numVertexes);
    staticMeshFile.read((char*)mVertexes.data(), numVertexes * sizeof(FStaticMeshVertex));

    int32 numIndexes;
    staticMeshFile.read((char*)&numIndexes, sizeof(int32));
    mIndexes.resize(numIndexes);
    staticMeshFile.read((char*)mIndexes.data(), numIndexes * sizeof(uint16));

    staticMeshFile.close();
}

void UStaticMesh::Unload()
{
    mVertexes.clear();
    mIndexes.clear();
    mVertexLayout.Elements.clear();
}
