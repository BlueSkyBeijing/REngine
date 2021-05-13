#include "PrecompiledHeader.h"

#include "USkeletalMesh.h"
#include "FRHI.h"
#include "FConfigManager.h"

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
    { "TEXCOORD", 0, EPixelFormat::PF_R32G32_FLOAT, 0, 24, ICF_PerVertexData, 0 },
    { "WEIGHTS", 0, EPixelFormat::PF_R32G32B32A32_FLOAT, 0, 32, ICF_PerVertexData, 0 },
    { "BONEINDICES", 0, EPixelFormat::PF_R8G8B8A8_UINT, 0, 48, ICF_PerVertexData, 0 }};


    layout.Elements.push_back(inputLayout[0]);
    layout.Elements.push_back(inputLayout[1]);
    layout.Elements.push_back(inputLayout[2]);
    layout.Elements.push_back(inputLayout[3]);
    layout.Elements.push_back(inputLayout[4]);

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
    skeletalMeshFile.read((char*)mVertexes.data(), numVertexes * sizeof(FSkeletalMeshVertex));

    int32 numIndexes;
    skeletalMeshFile.read((char*)&numIndexes, sizeof(int32));
    mIndexes.resize(numIndexes);
    skeletalMeshFile.read((char*)mIndexes.data(), numIndexes * sizeof(uint16));

    std::string ResourceName;
    int32 stringSize;
    skeletalMeshFile.read((char*)&stringSize, sizeof(int32));
    skeletalMeshFile.read((char*)ResourceName.data(), stringSize);

    FullSkeletonPath = FConfigManager::DefaultStaticMeshPath +
        std::string(ResourceName.c_str()) +
        FConfigManager::DefaultSkeletalMeshFileSuffix;
    skeletalMeshFile.close();
}

void USkeletalMesh::Unload()
{
    mVertexes.clear();
    mIndexes.clear();
    mVertexLayout.Elements.clear();
}


USkeleton::USkeleton()
{
}

USkeleton::~USkeleton()
{
}

void USkeleton::Load()
{
}

void USkeleton::Unload()
{
}

UAnimSequence::UAnimSequence()
{
}

UAnimSequence::~UAnimSequence()
{
}

void UAnimSequence::Load()
{
    std::ifstream animSequenceFile(FullFilePathName, std::ios::in | std::ios::binary);
    if (!animSequenceFile)
    {
        //print error
        return;
    }

    animSequenceFile.read((char*)&NumberOfFrames, sizeof(int32));
    PosKeys.resize(NumberOfFrames);
    RotKeys.resize(NumberOfFrames);
    ScaleKeys.resize(NumberOfFrames);

    animSequenceFile.close();

}

void UAnimSequence::Unload()
{
}

void UAnimSequence::Update(float deltaSeconds)
{
    //get 2 frame

    //interpolate
}
