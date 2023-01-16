#pragma once
#include "Prerequisite.h"

#include "UObject.h"
#include "FRHIVertex.h"


class USkeletalMesh : public UResource
{
public:
    USkeletalMesh();
    virtual ~USkeletalMesh() override;

public:
    FString FullSkeletonPath;

public:
    virtual void Load() override;
    virtual void Unload() override;

    inline const std::vector<FSkeletalMeshVertex>& GetVertexes() const
    {
        return mVertexes;
    }

    inline const std::vector<uint32>& GetIndexes() const
    {
        return mIndexes;
    }

    inline const FRHIVertexLayout& GetVertexLayout() const
    {
        return mVertexLayout;
    }

    inline const USkeleton* GetSkeleton() const
    {
        return mSkeleton;
    }

    inline const std::vector<FStaticMeshSection>& GetSections() const
    {
        return mSections;
    }

private:
    std::vector<FSkeletalMeshVertex> mVertexes;
    std::vector<uint32> mIndexes;
    FRHIVertexLayout mVertexLayout;
    std::vector<FStaticMeshSection> mSections;
    USkeleton* mSkeleton;
};

struct FBoneInfo
{
    FString Name;
    int32 ParentIndex;
};

struct FTransform
{
    FQuat Rotation;
    FVector3 Translation;
    FVector3 Scale3D;
};

struct FAnimSequenceTrack
{
    int32 BoneIndex;
    std::vector<FVector3> PosKeys;
    std::vector<FQuat> RotKeys;
    std::vector<FVector3> ScaleKeys;
};

class USkeleton : public UResource
{
public:
    USkeleton();
    virtual ~USkeleton() override;

public:
    virtual void Load() override;
    virtual void Unload() override;

    inline const std::vector<FBoneInfo>& GetBoneInfos() const
    {
        return mBoneInfos;
    }

    inline const std::vector<FTransform>& GetBonePose() const
    {
        return mBonePose;
    }

private:
    std::vector<FBoneInfo> mBoneInfos;
    std::vector<FTransform> mBonePose;
};

class UAnimSequence : public UResource
{
public:
    UAnimSequence();
    virtual ~UAnimSequence() override;

public:
    virtual void Load() override;
    virtual void Unload() override;

    inline const std::vector<FAnimSequenceTrack>& GetAnimSequenceTracks() const
    {
        return mAnimSequenceTracks;
    }

    inline const USkeleton* GeSkeleton() const
    {
        return mSkeleton;
    }

    inline void SeSkeleton(const USkeleton* skeleton)
    {
        mSkeleton = skeleton;
    }

    inline const int32 GetNumberOfFrames() const
    {
        return mNumberOfFrames;
    }

    inline const float GetSequenceLength() const
    {
        return mSequenceLength;
    }

private:
    int32 mNumberOfFrames;
    float mSequenceLength;
    std::vector<FAnimSequenceTrack> mAnimSequenceTracks;
    const USkeleton* mSkeleton;
};

class FAnimSequenceInstance
{
public:
    FAnimSequenceInstance(UAnimSequence* animSequence);
    ~FAnimSequenceInstance();

    void Update(float deltaSeconds);

    inline void ResetTime()
    {
        mElapsedSeconds = 0.0f;
    }

    std::vector<FMatrix4x4> BoneFinalTransforms;

private:
    float mElapsedSeconds;
    std::vector<FMatrix4x4> mBoneTransforms;
    UAnimSequence* mAnimSequence;
};

class FAnimSequenceBlender
{
public:
    FAnimSequenceBlender(UAnimSequence* anim0, UAnimSequence* anim1);
    ~FAnimSequenceBlender();

    void Blend(float time0, float weight0, float time1, float weight1);

    inline void SetAnimSequence(UAnimSequence* src, UAnimSequence* dest)
    {
        mAnimSequence0 = src;
        mAnimSequence1 = dest;
    }

public:
    std::vector<FMatrix4x4> BoneFinalTransforms;

private:
    UAnimSequence* mAnimSequence0;
    UAnimSequence* mAnimSequence1;
    const USkeleton* mSkeleton;
    std::vector<FMatrix4x4> mBoneTransforms;

    float mTime0;
    float mTime1;
};
