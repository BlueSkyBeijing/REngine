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
    { "WEIGHTS", 0, EPixelFormat::PF_R16G16B16A16_FLOAT, 0, 32, ICF_PerVertexData, 0 },
    { "BONEINDICES", 0, EPixelFormat::PF_R8G8B8A8_UINT, 0, 40, ICF_PerVertexData, 0 }};


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

    FullSkeletonPath = FConfigManager::DefaultSkeletonPath +
        std::string(ResourceName.c_str()) +
        FConfigManager::DefaultSkeletonFileSuffix;

    skeletalMeshFile.close();

    mSkeleton = new USkeleton();
    mSkeleton->FullFilePathName = FullSkeletonPath;
    mSkeleton->Load();
}

void USkeletalMesh::Unload()
{
    mVertexes.clear();
    mIndexes.clear();
    mVertexLayout.Elements.clear();

    mSkeleton->Unload();
    delete mSkeleton;
    mSkeleton = nullptr;
}


USkeleton::USkeleton()
{
}

USkeleton::~USkeleton()
{
}

void USkeleton::Load()
{
    std::ifstream skeletonFile(FullFilePathName, std::ios::in | std::ios::binary);
    if (!skeletonFile)
    {
        //print error
        return;
    }

    int32 numBoneInfos;
    int32 numPosBones;

    skeletonFile.read((char*)&numBoneInfos, sizeof(int32));

    mBoneInfos.resize(numBoneInfos);

    for (int32 i = 0; i < numBoneInfos; i++)
    {
        FBoneInfo data;
        //int32 stringSize;
        //skeletonFile.read((char*)&stringSize, sizeof(int32));
        //skeletonFile.read((char*)data.Name.data(), stringSize);

        skeletonFile.read((char*)&data.ParentIndex, sizeof(int32));

        mBoneInfos[i] = data;
    }

    //skeletonFile.read((char*)mBoneInfos.data(), numBoneInfos * sizeof(FBoneInfo));

    skeletonFile.read((char*)&numPosBones, sizeof(int32));

    mBonePose.resize(numPosBones);

    skeletonFile.read((char*)mBonePose.data(), numPosBones * sizeof(FTransform));

    skeletonFile.close();

}

void USkeleton::Unload()
{
}

UAnimSequence::UAnimSequence(const USkeleton * skeleton): mSkeleton(skeleton)
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

    mAnimSequenceTracks.resize(NumberOfFrames);


    for (int32 i = 0; i < NumberOfFrames; i++)
    {
        int32 numPosKeys;
        animSequenceFile.read((char*)&numPosKeys, sizeof(int32));
        mAnimSequenceTracks[i].PosKeys.resize(numPosKeys);
        animSequenceFile.read((char*)&numPosKeys, sizeof(int32));

        animSequenceFile.read((char*)mAnimSequenceTracks[i].PosKeys.data(), numPosKeys * sizeof(FVector3));

        int32 numRotKeys;
        animSequenceFile.read((char*)&numRotKeys, sizeof(int32));

        mAnimSequenceTracks[i].RotKeys.resize(numRotKeys);
        animSequenceFile.read((char*)&numPosKeys, sizeof(int32));

        animSequenceFile.read((char*)mAnimSequenceTracks[i].RotKeys.data(), numRotKeys * sizeof(FQuat));

        int32 numScaleKeys;
        animSequenceFile.read((char*)&numScaleKeys, sizeof(int32));

        mAnimSequenceTracks[i].ScaleKeys.resize(numScaleKeys);
        animSequenceFile.read((char*)&numPosKeys, sizeof(int32));

        animSequenceFile.read((char*)mAnimSequenceTracks[i].ScaleKeys.data(), numScaleKeys * sizeof(FVector3));
    }

    animSequenceFile.close();

    BoneFinalTransforms.resize(mSkeleton->GetBoneInfos().size());

}

void UAnimSequence::Unload()
{
}

void UAnimSequence::Update(float deltaSeconds)
{
    //get 2 frame

    //interpolate

    static int32 frameIndex = 0;

    // The root bone has index 0.  The root bone has no parent, so its toRootTransform
    // is just its local bone transform.
    BoneFinalTransforms[0].setIdentity();
    BoneFinalTransforms[0].block<3, 3>(0, 0) = mAnimSequenceTracks[0].RotKeys[frameIndex].toRotationMatrix();
    BoneFinalTransforms[0].block<1, 3>(3, 0) = mAnimSequenceTracks[0].PosKeys[frameIndex];

    // Now find the toRootTransform of the children.
    for (uint32 i = 1; i < mSkeleton->GetBoneInfos().size(); ++i)
    {
        FQuat toParentRot = mAnimSequenceTracks[i].RotKeys[frameIndex];
        //FVector3 toParentScale = mAnimSequenceTracks[i].ScaleKeys[frameIndex];
        FVector3 toParentTranslation = mAnimSequenceTracks[i].PosKeys[frameIndex];

        FMatrix4x4 toParent;
        toParent.setIdentity();
        toParent.block<3, 3>(0, 0) = toParentRot.toRotationMatrix();
        toParent.block<1, 3>(3, 0) = toParentTranslation;

        int32 parentIndex = mSkeleton->GetBoneInfos()[i].ParentIndex;
        FMatrix4x4& parentToRoot = BoneFinalTransforms[parentIndex];

        BoneFinalTransforms[i] = toParent * parentToRoot;
    }
    frameIndex++;

    //if (frameIndex >= mAnimSequenceTracks[0].RotKeys.size())
    //{
    //    frameIndex = frameIndex % mAnimSequenceTracks[0].RotKeys.size();
    //}
}
