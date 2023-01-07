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
    const static std::string DefaultSkeletalMeshPath;
    const static std::string DefaultSkeletonPath;
    const static std::string DefaultAnimSequencePath;
    const static std::string DefaultMaterialPath;
    const static std::string DefaultTexturePath;

    const static std::string DefaultMapFileSuffix;
    const static std::string DefaultStaticMeshFileSuffix;
    const static std::string DefaultSkeletalMeshFileSuffix;
    const static std::string DefaultSkeletonFileSuffix;
    const static std::string DefaultAnimSequenceFileSuffix;
    const static std::string DefaultMaterialFileSuffix;
    const static std::string DefaultTextureFileSuffix;

    const static int32 FrameBufferNum;

    static std::string DefaultMap;
    static int32 WindowWidth;
    static int32 WindowHeight;
    static int32 ShadowMapWidth;
    static int32 ShadowMapHeight;

    static std::string KeyNum1Cmd;
    static std::string KeyNum2Cmd;
    static std::string KeyNum3Cmd;
    static std::string KeyNum4Cmd;
    static std::string KeyNum5Cmd;
    static std::string KeyNum6Cmd;
    static std::string KeyNum7Cmd;
    static std::string KeyNum8Cmd;
    static std::string KeyNum9Cmd;
    static std::string KeyNum0Cmd;

private:
    FConfigManager();
    virtual ~FConfigManager();

};
