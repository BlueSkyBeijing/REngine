#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class DLL_API FConfigManager
{
    friend class TSingleton<FConfigManager>;

public:
    void Init();
    void UnInit();

    static FString Platform;
    static FString Configuration;

    static FString EngineDir;
    static FString EngineContentDir;
    static FString EngineShaderDir;
    static FString EngineConfigFile;
    static FString LogFile;

    static FString ProjectName;
    static FString ProjectDir;
    static FString ProjectContentDir;
    static FString ProjectShaderDir;
    static FString ProjectConfigFile;

    static FString DefaultMapPath;
    static FString DefaultStaticMeshPath;
    static FString DefaultSkeletalMeshPath;
    static FString DefaultSkeletonPath;
    static FString DefaultAnimSequencePath;
    static FString DefaultMaterialPath;
    static FString DefaultTexturePath;

    const static FString ProjectFileSuffix;
    const static FString DefaultMapFileSuffix;
    const static FString DefaultStaticMeshFileSuffix;
    const static FString DefaultSkeletalMeshFileSuffix;
    const static FString DefaultSkeletonFileSuffix;
    const static FString DefaultAnimSequenceFileSuffix;
    const static FString DefaultMaterialFileSuffix;
    const static FString DefaultTextureFileSuffix;

    const static int32 FrameBufferNum;

    static FString DefaultMap;
    static int32 WindowWidth;
    static int32 WindowHeight;
    static int32 ShadowMapWidth;
    static int32 ShadowMapHeight;
    static int32 MSAACount;

    static FString KeyNum1Cmd;
    static FString KeyNum2Cmd;
    static FString KeyNum3Cmd;
    static FString KeyNum4Cmd;
    static FString KeyNum5Cmd;
    static FString KeyNum6Cmd;
    static FString KeyNum7Cmd;
    static FString KeyNum8Cmd;
    static FString KeyNum9Cmd;
    static FString KeyNum0Cmd;

private:
    FConfigManager();
    virtual ~FConfigManager();

};
