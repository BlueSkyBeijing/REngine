#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FConfigManager : TSingleton<FConfigManager>
{
public:
    FConfigManager();
    ~FConfigManager();

    static std::string ContentPath;

    static std::string DefaultMapPath;
    static std::string DefaultStaticMeshPath;
    static std::string DefaultMaterialPath;
    static std::string DefaultTexturePath;

    static std::string DefaultMapFileSuffix;
    static std::string DefaultStaticMeshFileSuffix;
    static std::string DefaultMaterialFileSuffix;
    static std::string DefaultTextureFileSuffix;

private:

};
