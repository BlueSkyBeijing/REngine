#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FConfigManager
{
    friend class TSingleton<FConfigManager>;

public:
    void Init();
    void UnInit();

    const static std::string ContentPath;

    const static std::string DefaultMapPath;
    const static std::string DefaultStaticMeshPath;
    const static std::string DefaultMaterialPath;
    const static std::string DefaultTexturePath;

    const static std::string DefaultMapFileSuffix;
    const static std::string DefaultStaticMeshFileSuffix;
    const static std::string DefaultMaterialFileSuffix;
    const static std::string DefaultTextureFileSuffix;

    const static int32 FrameBufferNum;

    static std::string DefaultMap;
    static int32 WindowWidth;
    static int32 WindowHeight;
    static int32 ShadowMapWidth;
    static int32 ShadowMapHeight;

private:
    FConfigManager();
    virtual ~FConfigManager();

};
