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
    { "BINORMAL", 0, EPixelFormat::PF_R32G32B32_FLOAT, 0, 24, ICF_PerVertexData, 0 },
    { "TEXCOORD", 0, EPixelFormat::PF_R32G32_FLOAT, 0, 36, ICF_PerVertexData, 0 },
    { "BONEINDICES", 0, EPixelFormat::PF_R16G16B16A16_UINT, 0, 44, ICF_PerVertexData, 0 },
    { "WEIGHTS", 0, EPixelFormat::PF_R32G32B32A32_FLOAT, 0, 52, ICF_PerVertexData, 0 } };


    layout.Elements.push_back(inputLayout[0]);
    layout.Elements.push_back(inputLayout[1]);
    layout.Elements.push_back(inputLayout[2]);
    layout.Elements.push_back(inputLayout[3]);
    layout.Elements.push_back(inputLayout[4]);
    layout.Elements.push_back(inputLayout[5]);

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
    skeletalMeshFile.read((char*)mIndexes.data(), numIndexes * sizeof(uint32));

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

UAnimSequence::UAnimSequence(): mSkeleton(nullptr)
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

    animSequenceFile.read((char*)&mNumberOfFrames, sizeof(int32));
    animSequenceFile.read((char*)&mSequenceLength, sizeof(float));
    
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
}

void UAnimSequence::Unload()
{
}

FAnimSequenceInstance::FAnimSequenceInstance(UAnimSequence* animSequence):
    mAnimSequence(animSequence)
{
    BoneFinalTransforms.resize(animSequence->GeSkeleton()->GetBoneInfos().size());
    mBoneTransforms.resize(animSequence->GeSkeleton()->GetBoneInfos().size());
}

FAnimSequenceInstance::~FAnimSequenceInstance()
{
}

void FAnimSequenceInstance::Update(float deltaSeconds)
{
    mElapsedSeconds += deltaSeconds;

    const float sequenceTime = std::fmodf(mElapsedSeconds, mAnimSequence->GetSequenceLength());
    const float percentTime = sequenceTime / mAnimSequence->GetSequenceLength();
    const int32 keyFrame0 = static_cast<int32>(std::floorf(percentTime * (mAnimSequence->GetNumberOfFrames() - 1)));
    const int32 keyFrame1 = static_cast<int32>(std::ceilf(percentTime * (mAnimSequence->GetNumberOfFrames() - 1)));
    const float oneFrameTime = mAnimSequence->GetSequenceLength() / mAnimSequence->GetNumberOfFrames();
    const float lerpFrames = (sequenceTime - oneFrameTime * keyFrame0) / oneFrameTime;

    const std::vector<FBoneInfo>& boneInfos = mAnimSequence->GeSkeleton()->GetBoneInfos();
    const std::vector<FTransform>& bonePose = mAnimSequence->GeSkeleton()->GetBonePose();

    for (uint32 i = 0; i < boneInfos.size(); ++i)
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
        const int32 rotKeyNum = static_cast<int32>(mAnimSequence->GetAnimSequenceTracks()[i].RotKeys.size());
        if (rotKeyNum > 0)
        {
            rotKeyFrame0 = static_cast<int32>(std::floorf(percentTime * (rotKeyNum - 1)));

            toParentRot = mAnimSequence->GetAnimSequenceTracks()[i].RotKeys[rotKeyFrame0];

            if (rotKeyNum > 1)
            {
                rotKeyFrame1 = static_cast<int32>(std::ceilf(percentTime * (rotKeyNum - 1)));

                toParentRot = toParentRot.slerp(1.0f - lerpFrames, mAnimSequence->GetAnimSequenceTracks()[i].RotKeys[rotKeyFrame1]);
            }
        }

        const int32 posKeyNum = static_cast<int32>(mAnimSequence->GetAnimSequenceTracks()[i].PosKeys.size());
        if (posKeyNum > 0)
        {
            posKeyFrame0 = static_cast<int32>(std::floorf(percentTime * (posKeyNum - 1)));

            toParentTranslation = mAnimSequence->GetAnimSequenceTracks()[i].PosKeys[posKeyFrame0];

            if (posKeyNum > 1)
            {
                posKeyFrame1 = static_cast<int32>(std::ceilf(percentTime * (posKeyNum - 1)));

                toParentTranslation = toParentTranslation * lerpFrames + mAnimSequence->GetAnimSequenceTracks()[i].PosKeys[posKeyFrame1] * (1.0f - lerpFrames);
            }
        }

        BoneFinalTransforms[i].setIdentity();
        BoneFinalTransforms[i].block<3, 3>(0, 0) = toParentRot.toRotationMatrix();
        BoneFinalTransforms[i].block<3, 1>(0, 3) = toParentTranslation;

        if (i > 0)
        {
            const int32 parentIndex = boneInfos[i].ParentIndex;
            FMatrix4x4& parentToRoot = BoneFinalTransforms[parentIndex];

            BoneFinalTransforms[i] = parentToRoot * BoneFinalTransforms[i];
        }
    }

    for (uint32 i = 0; i < boneInfos.size(); ++i)
    {
        mBoneTransforms[i].setIdentity();
        mBoneTransforms[i].block<3, 3>(0, 0) = bonePose[i].Rotation.toRotationMatrix();
        mBoneTransforms[i].block<3, 1>(0, 3) = bonePose[i].Translation;

        if (i > 0)
        {
            const int32 parentIndex = boneInfos[i].ParentIndex;
            mBoneTransforms[i] = mBoneTransforms[parentIndex] * mBoneTransforms[i];
        }
    }

    for (uint32 i = 0; i < boneInfos.size(); ++i)
    {
        BoneFinalTransforms[i] = (BoneFinalTransforms[i] * mBoneTransforms[i].inverse()).transpose();
    }
}


FAnimSequenceBlender::FAnimSequenceBlender(UAnimSequence* anim0, UAnimSequence* anim1): mAnimSequence0(anim0), mAnimSequence1(anim1)
{
    mSkeleton = anim0->GeSkeleton();
    mBoneTransforms.resize(mSkeleton->GetBoneInfos().size());
    BoneFinalTransforms.resize(mSkeleton->GetBoneInfos().size());
    
}

FAnimSequenceBlender::~FAnimSequenceBlender()
{
}

void FAnimSequenceBlender::Blend(float time0, float weight0, float time1, float weight1)
{
    const float sequenceTimeA = std::fmodf(time0, mAnimSequence0->GetSequenceLength());
    const float percentTimeA = sequenceTimeA / mAnimSequence0->GetSequenceLength();
    const int32 keyFrame0A = static_cast<int32>(std::floorf(percentTimeA * (mAnimSequence0->GetNumberOfFrames() - 1)));
    const int32 keyFrame1A = static_cast<int32>(std::ceilf(percentTimeA * (mAnimSequence0->GetNumberOfFrames() - 1)));
    const float oneFrameTimeA = mAnimSequence0->GetSequenceLength() / mAnimSequence0->GetNumberOfFrames();
    const float lerpFramesA = (sequenceTimeA - oneFrameTimeA * keyFrame0A) / oneFrameTimeA;

    const float sequenceTimeB = std::fmodf(time1, mAnimSequence1->GetSequenceLength());
    const float percentTimeB = sequenceTimeB / mAnimSequence1->GetSequenceLength();
    const int32 keyFrame0B = static_cast<int32>(std::floorf(percentTimeB * (mAnimSequence1->GetNumberOfFrames() - 1)));
    const int32 keyFrame1B = static_cast<int32>(std::ceilf(percentTimeB * (mAnimSequence1->GetNumberOfFrames() - 1)));
    const float oneFrameTimeB = mAnimSequence1->GetSequenceLength() / mAnimSequence1->GetNumberOfFrames();
    const float lerpFramesB = (sequenceTimeB - oneFrameTimeB * keyFrame0B) / oneFrameTimeB;

    const std::vector<FBoneInfo>& boneInfos = mSkeleton->GetBoneInfos();
    const std::vector<FTransform>& bonePose = mSkeleton->GetBonePose();

    for (uint32 i = 0; i < boneInfos.size(); ++i)
    {
        int32 rotKeyFrame0A = 0;
        int32 rotKeyFrame1A = 0;

        int32 posKeyFrame0A = 0;
        int32 posKeyFrame1A = 0;

        FQuat toParentRotA;
        toParentRotA.setIdentity();
        FVector3 toParentTranslationA;
        toParentTranslationA.setZero();

        //get 2 frame and interpolate
        const std::vector<FAnimSequenceTrack>& animSequenceTracksA = mAnimSequence0->GetAnimSequenceTracks();
        const int32 rotKeyNumA = static_cast<int32>(animSequenceTracksA[i].RotKeys.size());
        if (rotKeyNumA > 0)
        {
            rotKeyFrame0A = static_cast<int32>(std::floorf(percentTimeA * (rotKeyNumA - 1)));

            toParentRotA = animSequenceTracksA[i].RotKeys[rotKeyFrame0A];

            if (rotKeyNumA > 1)
            {
                rotKeyFrame1A = static_cast<int32>(std::ceilf(percentTimeA * (rotKeyNumA - 1)));

                toParentRotA = toParentRotA.slerp(1.0f - lerpFramesA, animSequenceTracksA[i].RotKeys[rotKeyFrame1A]);
            }
        }

        const int32 posKeyNumA = static_cast<int32>(animSequenceTracksA[i].PosKeys.size());
        if (posKeyNumA > 0)
        {
            posKeyFrame0A = static_cast<int32>(std::floorf(percentTimeA * (posKeyNumA - 1)));

            toParentTranslationA = animSequenceTracksA[i].PosKeys[posKeyFrame0A];

            if (posKeyNumA > 1)
            {
                posKeyFrame1A = static_cast<int32>(std::ceilf(percentTimeA * (posKeyNumA - 1)));

                toParentTranslationA = toParentTranslationA * lerpFramesA + animSequenceTracksA[i].PosKeys[posKeyFrame1A] * (1.0f - lerpFramesA);
            }
        }


        int32 rotKeyFrame0B = 0;
        int32 rotKeyFrame1B = 0;

        int32 posKeyFrame0B = 0;
        int32 posKeyFrame1B = 0;

        FQuat toParentRotB;
        toParentRotB.setIdentity();
        FVector3 toParentTranslationB;
        toParentTranslationB.setZero();

        //get 2 frame and interpolate
        const std::vector<FAnimSequenceTrack>& animSequenceTracksB = mAnimSequence1->GetAnimSequenceTracks();
        const int32 rotKeyNumB = static_cast<int32>(animSequenceTracksB[i].RotKeys.size());
        if (rotKeyNumB > 0)
        {
            rotKeyFrame0B = static_cast<int32>(std::floorf(percentTimeB * (rotKeyNumB - 1)));

            toParentRotB = animSequenceTracksB[i].RotKeys[rotKeyFrame0B];

            if (rotKeyNumB > 1)
            {
                rotKeyFrame1B = static_cast<int32>(std::ceilf(percentTimeB * (rotKeyNumB - 1)));

                toParentRotB = toParentRotB.slerp(1.0f - lerpFramesB, animSequenceTracksB[i].RotKeys[rotKeyFrame1B]);
            }
        }

        const int32 posKeyNumB = static_cast<int32>(animSequenceTracksB[i].PosKeys.size());
        if (posKeyNumB > 0)
        {
            posKeyFrame0B = static_cast<int32>(std::floorf(percentTimeB * (posKeyNumB - 1)));

            toParentTranslationB = animSequenceTracksB[i].PosKeys[posKeyFrame0B];

            if (posKeyNumB > 1)
            {
                posKeyFrame1B = static_cast<int32>(std::ceilf(percentTimeB * (posKeyNumB - 1)));

                toParentTranslationB = toParentTranslationB * lerpFramesB + animSequenceTracksB[i].PosKeys[posKeyFrame1B] * (1.0f - lerpFramesB);
            }
        }

        FQuat toParentRot;
        toParentRot.setIdentity();
        FVector3 toParentTranslation;
        toParentTranslation.setZero();

        toParentRot = toParentRotA.slerp(1.0f - weight0, toParentRotB);
        toParentTranslation = toParentTranslationA * weight0 + toParentTranslationB * (1.0f - weight0);

        BoneFinalTransforms[i].setIdentity();
        BoneFinalTransforms[i].block<3, 3>(0, 0) = toParentRot.toRotationMatrix();
        BoneFinalTransforms[i].block<3, 1>(0, 3) = toParentTranslation;

        if (i > 0)
        {
            const int32 parentIndex = boneInfos[i].ParentIndex;
            FMatrix4x4& parentToRoot = BoneFinalTransforms[parentIndex];

            BoneFinalTransforms[i] = parentToRoot * BoneFinalTransforms[i];
        }
    }

    for (uint32 i = 0; i < boneInfos.size(); ++i)
    {
        mBoneTransforms[i].setIdentity();
        mBoneTransforms[i].block<3, 3>(0, 0) = bonePose[i].Rotation.toRotationMatrix();
        mBoneTransforms[i].block<3, 1>(0, 3) = bonePose[i].Translation;

        if (i > 0)
        {
            const int32 parentIndex = boneInfos[i].ParentIndex;
            mBoneTransforms[i] = mBoneTransforms[parentIndex] * mBoneTransforms[i];
        }
    }

    for (uint32 i = 0; i < boneInfos.size(); ++i)
    {
        BoneFinalTransforms[i] = (BoneFinalTransforms[i] * mBoneTransforms[i].inverse()).transpose();
    }

}
