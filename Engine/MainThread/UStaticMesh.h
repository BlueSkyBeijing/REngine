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

    inline const std::vector<FStaticMeshVertex>& GetVertexes() const
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

    inline const std::vector<FStaticMeshSection>& GetSections() const
    {
        return mSections;
    }

private:
    std::vector<FStaticMeshVertex> mVertexes;
    std::vector<uint32> mIndexes;
    FRHIVertexLayout mVertexLayout;
    std::vector<FStaticMeshSection> mSections;
};
