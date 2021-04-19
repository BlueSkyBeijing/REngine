#include "PrecompiledHeader.h"

#include "FConfigManager.h"

const std::string FConfigManager::ContentPath = "Content\\";

const std::string FConfigManager::DefaultMapPath = "Content\\Map\\";
const std::string FConfigManager::DefaultStaticMeshPath = "Content\\StaticMesh\\";
const std::string FConfigManager::DefaultMaterialPath = "Content\\Material\\";
const std::string FConfigManager::DefaultTexturePath = "Content\\Texture\\";

const std::string FConfigManager::DefaultMapFileSuffix = ".map";
const std::string FConfigManager::DefaultStaticMeshFileSuffix = ".stm";
const std::string FConfigManager::DefaultMaterialFileSuffix = ".mat";
const std::string FConfigManager::DefaultTextureFileSuffix = ".dds";

const int32 FConfigManager::FrameBufferNum = 2;

std::string FConfigManager::DefaultMap = "Default.map";

int32 FConfigManager::WindowWidth = 1366;
int32 FConfigManager::WindowHeight = 768;


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

    std::stringstream streamWidth(engineConfig.GetValue("Render", "WindowWidth", "1024"));
    streamWidth >> WindowWidth;

    std::stringstream streamHeight(engineConfig.GetValue("Render", "WindowHeight", "768"));
    streamHeight >> WindowHeight;
}

void FConfigManager::UnInit()
{
}
