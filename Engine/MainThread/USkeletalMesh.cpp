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
    { "BONEINDICES", 0, EPixelFormat::PF_R16G16B16A16_UINT, 0, 32, ICF_PerVertexData, 0 },
    { "WEIGHTS", 0, EPixelFormat::PF_R32G32B32A32_FLOAT, 0, 40, ICF_PerVertexData, 0 } };


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

        skeletonFile.read((char*)&data.ParentIndex, sizeof(int32));

        mBoneInfos[i] = data;
    }

    skeletonFile.read((char*)&numPosBones, sizeof(int32));

    mBonePose.resize(numPosBones);
    for (int32 i = 0; i < numBoneInfos; i++)
    {
        FTransform data;

        skeletonFile.read((char*)&data.Rotation, sizeof(FQuat));
        skeletonFile.read((char*)&data.Translation, sizeof(FVector3));
        skeletonFile.read((char*)&data.Scale3D, sizeof(FVector3));

        mBonePose[i] = data;
    }

    skeletonFile.close();

}

void USkeleton::Unload()
{
}

UAnimSequence::UAnimSequence(const USkeleton * skeleton): mSkeleton(skeleton), mElapsedSeconds(0.0f)
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
    animSequenceFile.read((char*)&SequenceLength, sizeof(float));
    
    mAnimSequenceTracks.resize(mSkeleton->GetBoneInfos().size());

    for (int32 i = 0; i < mSkeleton->GetBoneInfos().size(); i++)
    { 
        animSequenceFile.read((char*)&mAnimSequenceTracks[i].BoneIndex, sizeof(int32));

        int32 numPosKeys;
        animSequenceFile.read((char*)&numPosKeys, sizeof(int32));
        mAnimSequenceTracks[i].PosKeys.resize(numPosKeys);
        animSequenceFile.read((char*)mAnimSequenceTracks[i].PosKeys.data(), numPosKeys * sizeof(FVector3));

        int32 numRotKeys;
        animSequenceFile.read((char*)&numRotKeys, sizeof(int32));
        mAnimSequenceTracks[i].RotKeys.resize(numRotKeys);
        animSequenceFile.read((char*)mAnimSequenceTracks[i].RotKeys.data(), numRotKeys * sizeof(FQuat));

        int32 numScaleKeys;
        animSequenceFile.read((char*)&numScaleKeys, sizeof(int32));
        mAnimSequenceTracks[i].ScaleKeys.resize(numScaleKeys);
        animSequenceFile.read((char*)mAnimSequenceTracks[i].ScaleKeys.data(), numScaleKeys * sizeof(FVector3));
    }

    animSequenceFile.close();

    BoneFinalTransforms.resize(mSkeleton->GetBoneInfos().size());
    mBoneTransforms.resize(mSkeleton->GetBoneInfos().size());

}

void UAnimSequence::Unload()
{
}

void UAnimSequence::Update(float deltaSeconds)
{
    mElapsedSeconds += deltaSeconds;

    float sequenceTime = std::fmodf(mElapsedSeconds, SequenceLength);
    float percentTime = sequenceTime / SequenceLength;
    const int32 keyFrame0 = static_cast<int32>(std::floorf(percentTime * (NumberOfFrames - 1)));
    const int32 keyFrame1 = static_cast<int32>(std::ceilf(percentTime * (NumberOfFrames - 1)));
    const float oneFrameTime = SequenceLength / NumberOfFrames;
    const float lerpFrames = (sequenceTime - oneFrameTime * keyFrame0) / oneFrameTime;

    for (uint32 i = 0; i < mSkeleton->GetBoneInfos().size(); ++i)
    {
        int32 rotKeyFrame0 = 0;
        int32 rotKeyFrame1 = 0;

        int32 posKeyFrame0 = 0;
        int32 posKeyFrame1 = 0;

        FQuat toParentRot;
        toParentRot.setIdentity();
        FVector3 toParentTranslation;
        toParentTranslation.setZero();

        //get 2 frame and interpolate
        const int32 rotKeyNum = static_cast<int32>(mAnimSequenceTracks[i].RotKeys.size());
        if (rotKeyNum > 0)
        {
            rotKeyFrame0 = static_cast<int32>(std::floorf(percentTime * (rotKeyNum -1)));

            toParentRot = mAnimSequenceTracks[i].RotKeys[rotKeyFrame0];

            if (rotKeyNum > 1)
            {
                rotKeyFrame1 = static_cast<int32>(std::ceilf(percentTime * (rotKeyNum - 1)));

                toParentRot = toParentRot.slerp(lerpFrames, mAnimSequenceTracks[i].RotKeys[rotKeyFrame1]);
            }
        }

        const int32 posKeyNum = static_cast<int32>(mAnimSequenceTracks[i].PosKeys.size());
        if (posKeyNum > 0)
        {
            posKeyFrame0 = static_cast<int32>(std::floorf(percentTime * (posKeyNum - 1)));

            toParentTranslation = mAnimSequenceTracks[i].PosKeys[posKeyFrame0];

            if (posKeyNum > 1)
            {
                posKeyFrame1 = static_cast<int32>(std::ceilf(percentTime * (posKeyNum - 1)));

                toParentTranslation = toParentTranslation * lerpFrames + mAnimSequenceTracks[i].PosKeys[posKeyFrame1] * (1.0f - lerpFrames);
            }
        }

        BoneFinalTransforms[i].setIdentity();
        BoneFinalTransforms[i].block<3, 3>(0, 0) = toParentRot.toRotationMatrix();
        BoneFinalTransforms[i].block<3, 1>(0, 3) = toParentTranslation;

        if (i > 0)
        {
            const int32 parentIndex = mSkeleton->GetBoneInfos()[i].ParentIndex;
            FMatrix4x4& parentToRoot = BoneFinalTransforms[parentIndex];

            BoneFinalTransforms[i] = parentToRoot * BoneFinalTransforms[i];
        }
    }

    for (uint32 i = 0; i < mSkeleton->GetBoneInfos().size(); ++i)
    {
        mBoneTransforms[i].setIdentity();
        mBoneTransforms[i].block<3, 3>(0, 0) = mSkeleton->GetBonePose()[i].Rotation.toRotationMatrix();
        mBoneTransforms[i].block<3, 1>(0, 3) = mSkeleton->GetBonePose()[i].Translation;

        if (i > 0)
        {
            const int32 parentIndex = mSkeleton->GetBoneInfos()[i].ParentIndex;
            mBoneTransforms[i] = mBoneTransforms[parentIndex] * mBoneTransforms[i];
        }
    }

    for (uint32 i = 0; i < mSkeleton->GetBoneInfos().size(); ++i)
    {
        BoneFinalTransforms[i] = (BoneFinalTransforms[i] * mBoneTransforms[i].inverse()).transpose();
    }
}
