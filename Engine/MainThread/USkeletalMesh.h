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

private:
    std::vector<FSkeletalMeshVertex> mVertexes;
    std::vector<uint16> mIndexes;
    FRHIVertexLayout mVertexLayout;
    USkeleton* mSkeleton;
};


class USkeleton : public UResource
{
public:
    USkeleton();
    virtual ~USkeleton() override;

public:
    virtual void Load() override;
    virtual void Unload() override;

private:
};

class UAnimSequence : public UResource
{
public:
    UAnimSequence();
    virtual ~UAnimSequence() override;

public:
    virtual void Load() override;
    virtual void Unload() override;
    void Update(float deltaSeconds);

private:
    int32 NumberOfFrames;
    std::vector<FVector3> PosKeys;
    std::vector<FQuat> RotKeys;
    std::vector<FVector3> ScaleKeys;

};
