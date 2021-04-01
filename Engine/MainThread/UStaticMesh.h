#pragma once

#include "UObject.h"
#include "FVertex.h"

#include <vector>
#include <string>

class UStaticMesh : public UResource
{
public:
	UStaticMesh();
	virtual ~UStaticMesh() override;

public:
    virtual void Load() override;
    virtual void Unload() override;

	const std::vector<FStaticMeshVertex>& GetVertexes() const
	{
		return mVertexes;
	}

    const std::vector<uint16>& GetIndexes() const
    {
        return mIndexes;
    }

    const FVertexLayout& GetVertexLayout() const
    {
        return mVertexLayout;
    }

private:
	std::vector<FStaticMeshVertex> mVertexes;
	std::vector<uint16> mIndexes;
	FVertexLayout mVertexLayout;
};
