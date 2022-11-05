#include "PrecompiledHeader.h"

#include "UStaticMesh.h"
#include "FRHI.h"


UStaticMesh::UStaticMesh()
{
    FullFilePathName = "Content\\StaticMesh\\SM_Rock.stm";
}

UStaticMesh::~UStaticMesh()
{
}

void UStaticMesh::Load()
{
    FRHIVertexLayout layout;
    FInputElementDesc inputLayout[] = {
    { "POSITION", 0, EPixelFormat::PF_R32G32B32_FLOAT, 0, 0,  ICF_PerVertexData, 0 },
    { "NORMAL", 0, EPixelFormat::PF_R32G32B32A32_FLOAT, 0, 12, ICF_PerVertexData, 0 },
    { "TANGENT", 0, EPixelFormat::PF_R32G32B32_FLOAT, 0, 28, ICF_PerVertexData, 0 },
    { "TEXCOORD", 0, EPixelFormat::PF_R32G32_FLOAT, 0, 40, ICF_PerVertexData, 0 } };

    layout.Elements.push_back(inputLayout[0]);
    layout.Elements.push_back(inputLayout[1]);
    layout.Elements.push_back(inputLayout[2]);
    layout.Elements.push_back(inputLayout[3]);

    mVertexLayout = layout;

    std::ifstream staticMeshFile(FullFilePathName, std::ios::in | std::ios::binary);
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
    staticMeshFile.read((char*)mIndexes.data(), numIndexes * sizeof(uint32));

    int32 numSection;
    staticMeshFile.read((char*)&numSection, sizeof(int32));
    for (int32 iSection = 0; iSection < numSection; iSection++)
    {
        FStaticMeshSection section;
        staticMeshFile.read((char*)&section, sizeof(FStaticMeshSection));

        mSections.push_back(section);
    }

    staticMeshFile.close();
}

void UStaticMesh::Unload()
{
    mVertexes.clear();
    mIndexes.clear();
    mVertexLayout.Elements.clear();
}
