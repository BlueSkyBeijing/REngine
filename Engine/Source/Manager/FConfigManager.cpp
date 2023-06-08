﻿#include "PrecompiledHeader.h"

#include "FConfigManager.h"
#include "Utility.h"


FString FConfigManager::Platform = FString("x86");
FString FConfigManager::Configuration = FString("Debug");

FString FConfigManager::EngineDir = FString("\\Engine\\");
FString FConfigManager::EngineContentDir = FString("Content\\");
FString FConfigManager::EngineShaderDir = FString("Shaders\\");
FString FConfigManager::EngineConfigFile = FString("Config\\Engine.ini");
FString FConfigManager::LogFile = FString("Saved\\Logs\\Engine.log");

FString FConfigManager::ProjectName = FString("");
FString FConfigManager::ProjectDir = FString("");
FString FConfigManager::ProjectContentDir = FString("Content\\");
FString FConfigManager::ProjectShaderDir = FString("Shaders\\");
FString FConfigManager::ProjectConfigFile = FString("Config\\Engine.ini");

FString FConfigManager::DefaultMapPath = FString("Content\\Map\\");
FString FConfigManager::DefaultStaticMeshPath = FString("Content\\StaticMesh\\");
FString FConfigManager::DefaultSkeletalMeshPath = FString("Content\\SkeletalMesh\\");
FString FConfigManager::DefaultSkeletonPath = FString("Content\\SkeletalMesh\\Skeleton\\");
FString FConfigManager::DefaultAnimSequencePath = FString("Content\\SkeletalMesh\\Animation\\");
FString FConfigManager::DefaultMaterialPath = FString("Content\\Material\\");
FString FConfigManager::DefaultTexturePath = FString("Content\\Texture\\");

const FString FConfigManager::ProjectFileSuffix = FString(".project");
const FString FConfigManager::DefaultMapFileSuffix = FString(".map");
const FString FConfigManager::DefaultStaticMeshFileSuffix = FString(".stm");
const FString FConfigManager::DefaultSkeletalMeshFileSuffix = FString(".skm");
const FString FConfigManager::DefaultSkeletonFileSuffix = FString(".skt");
const FString FConfigManager::DefaultAnimSequenceFileSuffix = FString(".anm");
const FString FConfigManager::DefaultMaterialFileSuffix = FString(".mtl");
const FString FConfigManager::DefaultTextureFileSuffix = FString(".dds");

const int32 FConfigManager::FrameBufferNum = 2;

FString FConfigManager::DefaultMap = FString("Default.map");

int32 FConfigManager::WindowWidth = 1366;
int32 FConfigManager::WindowHeight = 768;

int32 FConfigManager::ShadowMapWidth = 1024;
int32 FConfigManager::ShadowMapHeight = 1024;

int32 FConfigManager::MSAACount = 1;

FString FConfigManager::KeyNum1Cmd = FString("");
FString FConfigManager::KeyNum2Cmd = FString("");
FString FConfigManager::KeyNum3Cmd = FString("");
FString FConfigManager::KeyNum4Cmd = FString("");
FString FConfigManager::KeyNum5Cmd = FString("");
FString FConfigManager::KeyNum6Cmd = FString("");
FString FConfigManager::KeyNum7Cmd = FString("");
FString FConfigManager::KeyNum8Cmd = FString("");
FString FConfigManager::KeyNum9Cmd = FString("");
FString FConfigManager::KeyNum0Cmd = FString(""); 

FConfigManager::FConfigManager()
{
}

FConfigManager::~FConfigManager()
{
}

void FConfigManager::Init()
{
    bool hasProject = (FConfigManager::ProjectDir.size() > 0);

    char szFilePath[MAX_PATH + 1] = { 0 };
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    TArray<FString> subPaths;
    StringSplit(szFilePath, FConfigManager::EngineDir, subPaths);
    FConfigManager::EngineDir = subPaths[0] + FConfigManager::EngineDir;

    FConfigManager::EngineContentDir = FConfigManager::EngineDir + FConfigManager::EngineContentDir;
    FConfigManager::EngineShaderDir = FConfigManager::EngineDir + FConfigManager::EngineShaderDir;
    FConfigManager::EngineConfigFile = FConfigManager::EngineDir + FConfigManager::EngineConfigFile;

    FConfigManager::ProjectContentDir = FConfigManager::ProjectDir + FConfigManager::ProjectContentDir;
    FConfigManager::ProjectShaderDir = FConfigManager::ProjectDir + FConfigManager::ProjectShaderDir;
    FConfigManager::ProjectConfigFile = FConfigManager::ProjectDir + FConfigManager::ProjectConfigFile;

    FString LogDir = hasProject ? FConfigManager::ProjectDir : FConfigManager::EngineDir;
    FConfigManager::LogFile = LogDir + FConfigManager::LogFile;

    FString ContentDir = hasProject ? FConfigManager::ProjectDir : FConfigManager::EngineDir;
    FConfigManager::DefaultMapPath = ContentDir + FConfigManager::DefaultMapPath;
    FConfigManager::DefaultStaticMeshPath = ContentDir + FConfigManager::DefaultStaticMeshPath;
    FConfigManager::DefaultSkeletalMeshPath = ContentDir + FConfigManager::DefaultSkeletalMeshPath;
    FConfigManager::DefaultSkeletonPath = ContentDir + FConfigManager::DefaultSkeletonPath;
    FConfigManager::DefaultAnimSequencePath = ContentDir + FConfigManager::DefaultAnimSequencePath;
    FConfigManager::DefaultMaterialPath = ContentDir + FConfigManager::DefaultMaterialPath;
    FConfigManager::DefaultTexturePath = ContentDir + FConfigManager::DefaultTexturePath;

    CSimpleIniA engineConfig;
    engineConfig.SetUnicode();

    FString ConfigFile = hasProject ? FConfigManager::ProjectConfigFile : FConfigManager::EngineConfigFile;

    SI_Error rc = engineConfig.LoadFile(ConfigFile.c_str());
    if (rc < 0)
    {
        //print error
    };

    const char* defaultMap;
    defaultMap = engineConfig.GetValue("Map", "DefaultMap", "Default.map");
    if (defaultMap != nullptr)
    {
        FConfigManager::DefaultMap = defaultMap;
    }

    std::stringstream streamWindowWidth(engineConfig.GetValue("Render", "WindowWidth", "1024"));
    streamWindowWidth >> WindowWidth;

    std::stringstream streamWindowHeight(engineConfig.GetValue("Render", "WindowHeight", "768"));
    streamWindowHeight >> WindowHeight;

    std::stringstream streamShadowMapWidth(engineConfig.GetValue("Render", "ShadowMapWidth", "1024"));
    streamShadowMapWidth >> ShadowMapWidth;

    std::stringstream streamShadowMapHeight(engineConfig.GetValue("Render", "ShadowMapHeight", "1024"));
    streamShadowMapHeight >> ShadowMapHeight;

    std::stringstream streamMSAACount(engineConfig.GetValue("Render", "MSAACount", "1"));
    streamMSAACount >> MSAACount;

    std::stringstream streamKeyNum1Cmd(engineConfig.GetValue("Key", "KeyNum1Cmd", ""));
    streamKeyNum1Cmd >> FConfigManager::KeyNum1Cmd;
    ReplaceSubString(FConfigManager::KeyNum1Cmd, "\\_", " ");

    std::stringstream streamKeyNum2Cmd(engineConfig.GetValue("Key", "KeyNum2Cmd", ""));
    streamKeyNum2Cmd >> FConfigManager::KeyNum2Cmd;
    ReplaceSubString(FConfigManager::KeyNum2Cmd, "\\_", " ");

    std::stringstream streamKeyNum3Cmd(engineConfig.GetValue("Key", "KeyNum3Cmd", ""));
    streamKeyNum3Cmd >> FConfigManager::KeyNum3Cmd;
    ReplaceSubString(FConfigManager::KeyNum3Cmd, "\\_", " ");

    std::stringstream streamKeyNum4Cmd(engineConfig.GetValue("Key", "KeyNum4Cmd", ""));
    streamKeyNum4Cmd >> FConfigManager::KeyNum4Cmd;
    ReplaceSubString(FConfigManager::KeyNum4Cmd, "\\_", " ");

    std::stringstream streamKeyNum5Cmd(engineConfig.GetValue("Key", "KeyNum5Cmd", ""));
    streamKeyNum5Cmd >> FConfigManager::KeyNum5Cmd;
    ReplaceSubString(FConfigManager::KeyNum5Cmd, "\\_", " ");

    std::stringstream streamKeyNum6Cmd(engineConfig.GetValue("Key", "KeyNum6Cmd", ""));
    streamKeyNum6Cmd >> FConfigManager::KeyNum6Cmd;
    ReplaceSubString(FConfigManager::KeyNum6Cmd, "\\_", " ");

    std::stringstream streamKeyNum7Cmd(engineConfig.GetValue("Key", "KeyNum7Cmd", ""));
    streamKeyNum7Cmd >> FConfigManager::KeyNum7Cmd;
    ReplaceSubString(FConfigManager::KeyNum7Cmd, "\\_", " ");

    std::stringstream streamKeyNum8Cmd(engineConfig.GetValue("Key", "KeyNum8Cmd", ""));
    streamKeyNum8Cmd >> FConfigManager::KeyNum8Cmd;
    ReplaceSubString(FConfigManager::KeyNum8Cmd, "\\_", " ");

    std::stringstream streamKeyNum9Cmd(engineConfig.GetValue("Key", "KeyNum9Cmd", ""));
    streamKeyNum9Cmd >> FConfigManager::KeyNum9Cmd;
    ReplaceSubString(FConfigManager::KeyNum9Cmd, "\\_", " ");

    std::stringstream streamKeyNum0Cmd(engineConfig.GetValue("Key", "KeyNum0Cmd", ""));
    streamKeyNum0Cmd >> FConfigManager::KeyNum0Cmd;
    ReplaceSubString(FConfigManager::KeyNum0Cmd, "\\_", " ");
}

void FConfigManager::UnInit()
{
}
