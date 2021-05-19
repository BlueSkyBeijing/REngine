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
    std::string FullSkeletonPath;

public:
    virtual void Load() override;
    virtual void Unload() override;

    inline const std::vector<FSkeletalMeshVertex>& GetVertexes() const
    {
        return mVertexes;
    }

    inline const std::vector<uint16>& GetIndexes() const
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
private:
    std::vector<FSkeletalMeshVertex> mVertexes;
    std::vector<uint16> mIndexes;
    FRHIVertexLayout mVertexLayout;
    USkeleton* mSkeleton;
};

struct FBoneInfo
{
    std::string Name;
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
    UAnimSequence(const USkeleton* skeleton);
    virtual ~UAnimSequence() override;

public:
    virtual void Load() override;
    virtual void Unload() override;
    void Update(float deltaSeconds);

    inline void ResetTime()
    {
        mElapsedSeconds = 0.0f;
    }

    inline const std::vector<FAnimSequenceTrack>& Get() const
    {
        return mAnimSequenceTracks;
    }

    std::vector<FMatrix4x4> BoneFinalTransforms;

private:
    int32 NumberOfFrames;
    float SequenceLength;
    std::vector<FAnimSequenceTrack> mAnimSequenceTracks;
    const USkeleton* mSkeleton;

    std::vector<FMatrix4x4> mBoneTransforms;
    float mElapsedSeconds;
};
