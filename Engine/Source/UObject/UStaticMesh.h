#pragma once
#include "Prerequisite.h"

#include "UObject.h"
#include "FRHIVertex.h"


class UStaticMesh : public UResource
{
public:
    UStaticMesh();
    virtual ~UStaticMesh() override;

public:
    virtual void Load() override;
    virtual void Unload() override;

    inline const TArray<FStaticMeshVertex>& GetVertexes() const
    {
        return mVertexes;
    }

    inline const TArray<uint32>& GetIndexes() const
    {
        return mIndexes;
    }

    inline const FRHIVertexLayout& GetVertexLayout() const
    {
        return mVertexLayout;
    }

    inline const TArray<FStaticMeshSection>& GetSections() const
    {
        return mSections;
    }

private:
    TArray<FStaticMeshVertex> mVertexes;
    TArray<uint32> mIndexes;
    FRHIVertexLayout mVertexLayout;
    TArray<FStaticMeshSection> mSections;
};
