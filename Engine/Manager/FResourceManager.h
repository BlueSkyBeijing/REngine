#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

enum EResourceType
{
    RT_StaticMesh,
    RT_Texture,
    RT_Material,
    RT_Map,
    RT_MaxNum
};

class FResourceManager
{
    friend class TSingleton<FResourceManager>;

public:
    void Init();
    void UnInit();

    UObject* GetOrLoad();

private:
    FResourceManager();
    virtual ~FResourceManager();

};
