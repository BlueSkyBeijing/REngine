#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

enum EResourceType
{
    RT_StaticMesh,
    RT_SkeletalMesh,
    RT_Skeleton,
    RT_Animation,
    RT_Texture,
    RT_Material,
    RT_Map,
    RT_MaxNum
};

typedef std::map<std::string, UResource*> TResourceMap;

class FResourceManager
{
    friend class TSingleton<FResourceManager>;

public:
    void Init();
    void UnInit();

    UResource* GetOrCreate(EResourceType resorceType, const std::string& fullFileName);

private:
    FResourceManager();
    virtual ~FResourceManager();

private:
    TResourceMap mResourceMaps[RT_MaxNum];
};
