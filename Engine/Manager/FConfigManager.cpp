#include "PrecompiledHeader.h"

#include "FConfigManager.h"
#include "Utility.h"

const std::string FConfigManager::ContentPath = "Content\\";

const std::string FConfigManager::DefaultMapPath = "Content\\Map\\";
const std::string FConfigManager::DefaultStaticMeshPath = "Content\\StaticMesh\\";
const std::string FConfigManager::DefaultSkeletalMeshPath = "Content\\SkeletalMesh\\";
const std::string FConfigManager::DefaultSkeletonPath = "Content\\SkeletalMesh\\Skeleton\\";
const std::string FConfigManager::DefaultAnimSequencePath = "Content\\SkeletalMesh\\Animation\\";
const std::string FConfigManager::DefaultMaterialPath = "Content\\Material\\";
const std::string FConfigManager::DefaultTexturePath = "Content\\Texture\\";

const std::string FConfigManager::DefaultMapFileSuffix = ".map";
const std::string FConfigManager::DefaultStaticMeshFileSuffix = ".stm";
const std::string FConfigManager::DefaultSkeletalMeshFileSuffix = ".skm";
const std::string FConfigManager::DefaultSkeletonFileSuffix = ".skt";
const std::string FConfigManager::DefaultAnimSequenceFileSuffix = ".anm";
const std::string FConfigManager::DefaultMaterialFileSuffix = ".mtl";
const std::string FConfigManager::DefaultTextureFileSuffix = ".dds";

const int32 FConfigManager::FrameBufferNum = 2;

std::string FConfigManager::DefaultMap = "Default.map";

int32 FConfigManager::WindowWidth = 1366;
int32 FConfigManager::WindowHeight = 768;

int32 FConfigManager::ShadowMapWidth = 1024;
int32 FConfigManager::ShadowMapHeight = 1024;

std::string FConfigManager::KeyNum1Cmd = "";
std::string FConfigManager::KeyNum2Cmd = "";
std::string FConfigManager::KeyNum3Cmd = "";
std::string FConfigManager::KeyNum4Cmd = "";
std::string FConfigManager::KeyNum5Cmd = "";
std::string FConfigManager::KeyNum6Cmd = "";
std::string FConfigManager::KeyNum7Cmd = "";
std::string FConfigManager::KeyNum8Cmd = "";
std::string FConfigManager::KeyNum9Cmd = "";
std::string FConfigManager::KeyNum0Cmd = "";

FConfigManager::FConfigManager()
{
}

FConfigManager::~FConfigManager()
{
}

void FConfigManager::Init()
{
    CSimpleIniA engineConfig;
    engineConfig.SetUnicode();

    SI_Error rc = engineConfig.LoadFile("Config\\Engine.ini");
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
