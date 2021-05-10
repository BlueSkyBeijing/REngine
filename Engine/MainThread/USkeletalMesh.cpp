#include "PrecompiledHeader.h"

#include "USkeletalMesh.h"
#include "FRHI.h"


USkeletalMesh::USkeletalMesh()
{
    FullFilePathName = "Content\\StaticMesh\\SM_Rock.stm";
}

USkeletalMesh::~USkeletalMesh()
{
}

void USkeletalMesh::Load()
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

    std::ifstream skeletalMeshFile(FullFilePathName, std::ios::in | std::ios::binary);
    if (!skeletalMeshFile)
    {
        //print error
        return;
    }

    int32 numVertexes;
    skeletalMeshFile.read((char*)&numVertexes, sizeof(int32));
    mVertexes.resize(numVertexes);
    skeletalMeshFile.read((char*)mVertexes.data(), numVertexes * sizeof(FStaticMeshVertex));

    int32 numIndexes;
    skeletalMeshFile.read((char*)&numIndexes, sizeof(int32));
    mIndexes.resize(numIndexes);
    skeletalMeshFile.read((char*)mIndexes.data(), numIndexes * sizeof(uint16));

    skeletalMeshFile.close();
}

void USkeletalMesh::Unload()
{
    mVertexes.clear();
    mIndexes.clear();
    mVertexLayout.Elements.clear();
}
