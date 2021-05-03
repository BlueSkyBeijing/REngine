#include "PrecompiledHeader.h"

#include "UStaticMesh.h"
#include "FRHI.h"


UStaticMesh::UStaticMesh()
{
    FilePathName = "Content\\StaticMesh\\SM_Rock.stm";
}

UStaticMesh::~UStaticMesh()
{
}

void UStaticMesh::Load()
{
    FRHIVertexLayout layout;
    FInputElementDesc inputLayout[] = {
    { "POSITION", 0, EPixelFormat::PF_R32G32B32_FLOAT, 0, 0,  ICF_PerVertexData, 0 },
    { "NORMAL", 0, EPixelFormat::PF_R32G32B32_FLOAT, 0, 12, ICF_PerVertexData, 0 },
    { "TEXCOORD", 0, EPixelFormat::PF_R32G32_FLOAT, 0, 24, ICF_PerVertexData, 0 } };

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
