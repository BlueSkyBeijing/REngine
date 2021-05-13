#include "PrecompiledHeader.h"

#include "FConfigManager.h"

const std::string FConfigManager::ContentPath = "Content\\";

const std::string FConfigManager::DefaultMapPath = "Content\\Map\\";
const std::string FConfigManager::DefaultStaticMeshPath = "Content\\StaticMesh\\";
const std::string FConfigManager::DefaultSkeletalMeshPath = "Content\\SkeletalMesh\\";
const std::string FConfigManager::DefaultSkeletonPath = "Content\\SkeletalMesh\\Skeleton\\";
const std::string FConfigManager::DefaultAnimSequencePath = "Content\\SkeletalMesh\\AnimSequence\\";
const std::string FConfigManager::DefaultMaterialPath = "Content\\Material\\";
const std::string FConfigManager::DefaultTexturePath = "Content\\Texture\\";

const std::string FConfigManager::DefaultMapFileSuffix = ".map";
const std::string FConfigManager::DefaultStaticMeshFileSuffix = ".stm";
const std::string FConfigManager::DefaultSkeletalMeshFileSuffix = ".skm";
const std::string FConfigManager::DefaultSkeletonFileSuffix = ".skt";
const std::string FConfigManager::DefaultAnimSequenceFileSuffix = ".anm";
const std::string FConfigManager::DefaultMaterialFileSuffix = ".mat";
const std::string FConfigManager::DefaultTextureFileSuffix = ".dds";

const int32 FConfigManager::FrameBufferNum = 2;

std::string FConfigManager::DefaultMap = "Default.map";

int32 FConfigManager::WindowWidth = 1366;
int32 FConfigManager::WindowHeight = 768;

int32 FConfigManager::ShadowMapWidth = 1024;
int32 FConfigManager::ShadowMapHeight = 1024;

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

}

void FConfigManager::UnInit()
{
}
