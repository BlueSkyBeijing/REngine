#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FConfigManager
{
    friend class TSingleton<FConfigManager>;

public:
    void Init();
    void UnInit();

    const static FString ContentPath;

    const static FString DefaultMapPath;
    const static FString DefaultStaticMeshPath;
    const static FString DefaultSkeletalMeshPath;
    const static FString DefaultSkeletonPath;
    const static FString DefaultAnimSequencePath;
    const static FString DefaultMaterialPath;
    const static FString DefaultTexturePath;

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
