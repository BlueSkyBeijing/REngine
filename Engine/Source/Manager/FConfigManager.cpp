#include "PrecompiledHeader.h"

#include "FConfigManager.h"
#include "Utility.h"


FString FConfigManager::Platform = "x86";
FString FConfigManager::Configuration = "Debug";

FString FConfigManager::EngineDir = "\\Engine\\";
FString FConfigManager::EngineContentDir = "Content\\";
FString FConfigManager::EngineShaderDir = "Shaders\\";
FString FConfigManager::EngineConfigFile = "Config\\Engine.ini";
FString FConfigManager::LogFile = "Saved\\Logs\\Engine.log";

FString FConfigManager::ProjectName = "";
FString FConfigManager::ProjectDir = "";
FString FConfigManager::ProjectContentDir = "Content\\";
FString FConfigManager::ProjectShaderDir = "Shaders\\";
FString FConfigManager::ProjectConfigFile = "Config\\Engine.ini";

FString FConfigManager::DefaultMapPath = "Content\\Map\\";
FString FConfigManager::DefaultStaticMeshPath = "Content\\StaticMesh\\";
FString FConfigManager::DefaultSkeletalMeshPath = "Content\\SkeletalMesh\\";
FString FConfigManager::DefaultSkeletonPath = "Content\\SkeletalMesh\\Skeleton\\";
FString FConfigManager::DefaultAnimSequencePath = "Content\\SkeletalMesh\\Animation\\";
FString FConfigManager::DefaultMaterialPath = "Content\\Material\\";
FString FConfigManager::DefaultTexturePath = "Content\\Texture\\";

const FString FConfigManager::ProjectFileSuffix = ".project";
const FString FConfigManager::DefaultMapFileSuffix = ".map";
const FString FConfigManager::DefaultStaticMeshFileSuffix = ".stm";
const FString FConfigManager::DefaultSkeletalMeshFileSuffix = ".skm";
const FString FConfigManager::DefaultSkeletonFileSuffix = ".skt";
const FString FConfigManager::DefaultAnimSequenceFileSuffix = ".anm";
const FString FConfigManager::DefaultMaterialFileSuffix = ".mtl";
const FString FConfigManager::DefaultTextureFileSuffix = ".dds";

const int32 FConfigManager::FrameBufferNum = 2;

FString FConfigManager::DefaultMap = "Default.map";

int32 FConfigManager::WindowWidth = 1366;
int32 FConfigManager::WindowHeight = 768;

int32 FConfigManager::ShadowMapWidth = 1024;
int32 FConfigManager::ShadowMapHeight = 1024;

FString FConfigManager::KeyNum1Cmd = "";
FString FConfigManager::KeyNum2Cmd = "";
FString FConfigManager::KeyNum3Cmd = "";
FString FConfigManager::KeyNum4Cmd = "";
FString FConfigManager::KeyNum5Cmd = "";
FString FConfigManager::KeyNum6Cmd = "";
FString FConfigManager::KeyNum7Cmd = "";
FString FConfigManager::KeyNum8Cmd = "";
FString FConfigManager::KeyNum9Cmd = "";
FString FConfigManager::KeyNum0Cmd = "";

FConfigManager::FConfigManager()
{
}

FConfigManager::~FConfigManager()
{
}

void FConfigManager::Init()
{
    bool hasProject = (ProjectDir.size() > 0);

    char szFilePath[MAX_PATH + 1] = { 0 };
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    TArray<FString> subPaths;
    StringSplit(szFilePath, EngineDir, subPaths);
    EngineDir = subPaths[0] + EngineDir;

    EngineContentDir = EngineDir + EngineContentDir;
    EngineShaderDir = EngineDir + EngineShaderDir;
    EngineConfigFile = EngineDir + EngineConfigFile;

    ProjectContentDir = ProjectDir + ProjectContentDir;
    ProjectShaderDir = ProjectDir + ProjectShaderDir;
    ProjectConfigFile = ProjectDir + ProjectConfigFile;

    FString LogDir = hasProject ? ProjectDir : EngineDir;
    LogFile = LogDir + LogFile;

    FString ContentDir = hasProject ? ProjectDir : EngineDir;
    DefaultMapPath = ContentDir + DefaultMapPath;
    DefaultStaticMeshPath = ContentDir + DefaultStaticMeshPath;
    DefaultSkeletalMeshPath = ContentDir + DefaultSkeletalMeshPath;
    DefaultSkeletonPath = ContentDir + DefaultSkeletonPath;
    DefaultAnimSequencePath = ContentDir + DefaultAnimSequencePath;
    DefaultMaterialPath = ContentDir + DefaultMaterialPath;
    DefaultTexturePath = ContentDir + DefaultTexturePath;

    CSimpleIniA engineConfig;
    engineConfig.SetUnicode();

    FString ConfigFile = hasProject ? ProjectConfigFile : EngineConfigFile;

    SI_Error rc = engineConfig.LoadFile(ConfigFile.c_str());
    if (rc < 0)
    {
        //print error
    };

    const char* defaultMap;
    defaultMap = engineConfig.GetValue("Map", "DefaultMap", "Default.map");
    if (defaultMap != nullptr)
    {
        DefaultMap = defaultMap;
    }

    std::stringstream streamWindowWidth(engineConfig.GetValue("Render", "WindowWidth", "1024"));
    streamWindowWidth >> WindowWidth;

    std::stringstream streamWindowHeight(engineConfig.GetValue("Render", "WindowHeight", "768"));
    streamWindowHeight >> WindowHeight;

    std::stringstream streamShadowMapWidth(engineConfig.GetValue("Render", "ShadowMapWidth", "1024"));
    streamShadowMapWidth >> ShadowMapWidth;

    std::stringstream streamShadowMapHeight(engineConfig.GetValue("Render", "ShadowMapHeight", "1024"));
    streamShadowMapHeight >> ShadowMapHeight;

    std::stringstream streamKeyNum1Cmd(engineConfig.GetValue("Key", "KeyNum1Cmd", ""));
    streamKeyNum1Cmd >> KeyNum1Cmd;
    ReplaceSubString(KeyNum1Cmd, "\\_", " ");

    std::stringstream streamKeyNum2Cmd(engineConfig.GetValue("Key", "KeyNum2Cmd", ""));
    streamKeyNum2Cmd >> KeyNum2Cmd;
    ReplaceSubString(KeyNum2Cmd, "\\_", " ");

    std::stringstream streamKeyNum3Cmd(engineConfig.GetValue("Key", "KeyNum3Cmd", ""));
    streamKeyNum3Cmd >> KeyNum3Cmd;
    ReplaceSubString(KeyNum3Cmd, "\\_", " ");

    std::stringstream streamKeyNum4Cmd(engineConfig.GetValue("Key", "KeyNum4Cmd", ""));
    streamKeyNum4Cmd >> KeyNum4Cmd;
    ReplaceSubString(KeyNum4Cmd, "\\_", " ");

    std::stringstream streamKeyNum5Cmd(engineConfig.GetValue("Key", "KeyNum5Cmd", ""));
    streamKeyNum5Cmd >> KeyNum5Cmd;
    ReplaceSubString(KeyNum5Cmd, "\\_", " ");

    std::stringstream streamKeyNum6Cmd(engineConfig.GetValue("Key", "KeyNum6Cmd", ""));
    streamKeyNum6Cmd >> KeyNum6Cmd;
    ReplaceSubString(KeyNum6Cmd, "\\_", " ");

    std::stringstream streamKeyNum7Cmd(engineConfig.GetValue("Key", "KeyNum7Cmd", ""));
    streamKeyNum7Cmd >> KeyNum7Cmd;
    ReplaceSubString(KeyNum7Cmd, "\\_", " ");

    std::stringstream streamKeyNum8Cmd(engineConfig.GetValue("Key", "KeyNum8Cmd", ""));
    streamKeyNum8Cmd >> KeyNum8Cmd;
    ReplaceSubString(KeyNum8Cmd, "\\_", " ");

    std::stringstream streamKeyNum9Cmd(engineConfig.GetValue("Key", "KeyNum9Cmd", ""));
    streamKeyNum9Cmd >> KeyNum9Cmd;
    ReplaceSubString(KeyNum9Cmd, "\\_", " ");

    std::stringstream streamKeyNum0Cmd(engineConfig.GetValue("Key", "KeyNum0Cmd", ""));
    streamKeyNum0Cmd >> KeyNum0Cmd;
    ReplaceSubString(KeyNum0Cmd, "\\_", " ");
}

void FConfigManager::UnInit()
{
}
