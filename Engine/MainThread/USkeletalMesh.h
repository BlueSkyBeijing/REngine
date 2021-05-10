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
};
