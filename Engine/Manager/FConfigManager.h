#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FConfigManager : TSingleton<FConfigManager>
{
public:
    FConfigManager();
    ~FConfigManager();

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

private:

};
