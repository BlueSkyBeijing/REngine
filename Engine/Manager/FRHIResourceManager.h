#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

enum ERHIResourceType
{
    RHIRT_Texture2D,
    RHIRT_TextureCube,
    RHIRT_MaxNum
};

typedef std::map<std::string, FRHIResource*> TRHIResourceMap;

class FRHIResourceManager
{
    friend class TSingleton<FRHIResourceManager>;

public:
    void Init();
    void UnInit();

    FRHIResource* GetOrCreate(ERHIResourceType resorceType, const std::string& fullFileName);

private:
    FRHIResourceManager();
    virtual ~FRHIResourceManager();

private:
    TRHIResourceMap mResourceMaps[RHIRT_MaxNum];
};
