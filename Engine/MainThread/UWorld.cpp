#include "PrecompiledHeader.h"

#include "UWorld.h"
#include "FEngine.h"
#include "FRenderThread.h"
#include "UStaticMeshObject.h"
#include "USkeletalMeshObject.h"
#include "UCamera.h"
#include "ULight.h"
#include "Utility.h"
#include "FConfigManager.h"
#include "FRenderCommand.h"
#include "FPlayerController.h"
#include "UPlayer.h"

UWorld::UWorld(FEngine* engine) :
    mEngine(engine)
{
}

UWorld::~UWorld()
{
}

void UWorld::Load()
{
    FullFilePathName = TSingleton<FConfigManager>::GetInstance().DefaultMapPath +
        TSingleton<FConfigManager>::GetInstance().DefaultMap;

    //load from file
    std::ifstream mapFile(FullFilePathName, std::ios::in | std::ios::binary);
    if (!mapFile)
    {
        //print error
        return;
    }

    int32 numCamera;
    std::vector<FCameraData> cameraDatas;
    mapFile.read((char*)&numCamera, sizeof(int32));
    cameraDatas.resize(numCamera);
    mapFile.read((char*)cameraDatas.data(), numCamera * sizeof(FCameraData));

    int32 numDirectionalLight;
    std::vector<FDirectionalLightData> directionalLightDatas;
    mapFile.read((char*)&numDirectionalLight, sizeof(int32));
    directionalLightDatas.resize(numDirectionalLight);
    mapFile.read((char*)directionalLightDatas.data(), numDirectionalLight * sizeof(FDirectionalLightData));

    int32 numStaticMeshObject;
    std::vector<FStaticMeshObjectData> staticMeshObjectDatas;
    mapFile.read((char*)&numStaticMeshObject, sizeof(int32));
    for (int32 i = 0; i < numStaticMeshObject; i++)
    {
        FStaticMeshObjectData data;
        mapFile.read((char*)&data.Rotation, sizeof(FQuat));
        mapFile.read((char*)data.Location.data(), sizeof(FVector3));
        int32 stringSize;
        mapFile.read((char*)&stringSize, sizeof(int32));
        mapFile.read((char*)data.ResourceName.data(), stringSize);
        mapFile.read((char*)&stringSize, sizeof(int32));
        mapFile.read((char*)data.MaterialName.data(), stringSize);

        staticMeshObjectDatas.push_back(data);
    }

    int32 numSkeletalMeshObject;
    std::vector<FSkeletalMeshObjectData> skeletalMeshObjectDatas;
    mapFile.read((char*)&numSkeletalMeshObject, sizeof(int32));
    for (int32 i = 0; i < numSkeletalMeshObject; i++)
    {
        FSkeletalMeshObjectData data;
        mapFile.read((char*)&data.Rotation, sizeof(FQuat));
        mapFile.read((char*)data.Location.data(), sizeof(FVector3));
        int32 stringSize;
        mapFile.read((char*)&stringSize, sizeof(int32));
        mapFile.read((char*)data.ResourceName.data(), stringSize);
        int32 stringSizeAnim;
        mapFile.read((char*)&stringSizeAnim, sizeof(int32));
        mapFile.read((char*)data.AnimationName.data(), stringSizeAnim);
        mapFile.read((char*)&stringSize, sizeof(int32));
        mapFile.read((char*)data.MaterialName.data(), stringSize);

        skeletalMeshObjectDatas.push_back(data);
    }

    mapFile.close();

    //init from import data
    mCameras.resize(numCamera);
    int32 camerDataIndex = 0;
    for (auto it = mCameras.begin(); it != mCameras.end(); it++)
    {
        UCamera* camera = new UCamera();

        camera->Position = cameraDatas[camerDataIndex].Position;
        camera->Target = cameraDatas[camerDataIndex].Target;
        camera->FOV = cameraDatas[camerDataIndex].FOV;
        camera->AspectRatio = cameraDatas[camerDataIndex].AspectRatio;

        camera->Load();

        *it = camera;

        camerDataIndex++;
    }

    mDirectionalLights.resize(numDirectionalLight);
    int32 directionalLightDataIndex = 0;
    for (auto it = mDirectionalLights.begin(); it != mDirectionalLights.end(); it++)
    {
        UDirectionalLight* directionalLight = new UDirectionalLight();

        directionalLight->Color = directionalLightDatas[directionalLightDataIndex].Color;
        directionalLight->Direction = directionalLightDatas[directionalLightDataIndex].Direction;
        directionalLight->Intensity = directionalLightDatas[directionalLightDataIndex].Intensity;

        directionalLight->Load();

        *it = directionalLight;

        directionalLightDataIndex++;
    }

    mStaticMeshObjects.resize(numStaticMeshObject);
    int32 staticMeshObjectDataIndex = 0;
    for (auto it = mStaticMeshObjects.begin(); it != mStaticMeshObjects.end(); it++)
    {
        UStaticMeshObject* staticMeshObject = new UStaticMeshObject();

        staticMeshObject->Position = staticMeshObjectDatas[staticMeshObjectDataIndex].Location;
        staticMeshObject->Rotation = staticMeshObjectDatas[staticMeshObjectDataIndex].Rotation;
        staticMeshObject->FullResourcePath = FConfigManager::DefaultStaticMeshPath +
            std::string(staticMeshObjectDatas[staticMeshObjectDataIndex].ResourceName.c_str()) +
            FConfigManager::DefaultStaticMeshFileSuffix;
        staticMeshObject->Name = std::string(staticMeshObjectDatas[staticMeshObjectDataIndex].ResourceName.c_str());
        staticMeshObject->FullMaterialPath = FConfigManager::DefaultMaterialPath +
            std::string(staticMeshObjectDatas[staticMeshObjectDataIndex].MaterialName.c_str()) +
            FConfigManager::DefaultMaterialFileSuffix;

        staticMeshObject->Load();

        *it = staticMeshObject;

        staticMeshObjectDataIndex++;
    }

    mSkeletalMeshObjects.resize(numSkeletalMeshObject);
    int32 skeletalMeshObjectDataIndex = 0;
    for (auto it = mSkeletalMeshObjects.begin(); it != mSkeletalMeshObjects.end(); it++)
    {
        USkeletalMeshObject* skeletalMeshObject = new USkeletalMeshObject();

        skeletalMeshObject->Position = skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].Location;
        skeletalMeshObject->Rotation = skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].Rotation;
        skeletalMeshObject->FullResourcePath = FConfigManager::DefaultSkeletalMeshPath +
            std::string(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].ResourceName.c_str()) +
            FConfigManager::DefaultSkeletalMeshFileSuffix;
        skeletalMeshObject->Name = std::string(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].ResourceName.c_str());
        skeletalMeshObject->FullAnimSequencePath = FConfigManager::DefaultAnimSequencePath +
            std::string(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].AnimationName.c_str()) +
            FConfigManager::DefaultAnimSequenceFileSuffix;
        skeletalMeshObject->FullMaterialPath = FConfigManager::DefaultMaterialPath +
            std::string(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].MaterialName.c_str()) +
            FConfigManager::DefaultMaterialFileSuffix;

        skeletalMeshObject->Load();

        *it = skeletalMeshObject;

        skeletalMeshObjectDataIndex++;
    }

    //tell render thread load completed
    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();

    ENQUEUE_RENDER_COMMAND([renderThread]
    {
        renderThread->MarkLoadCompleted();
    });

    mPlayer = new UPlayer();
    mPlayer->FullMaterialPath = FConfigManager::DefaultMaterialPath +
        std::string(skeletalMeshObjectDatas[0].MaterialName.c_str()) +
        FConfigManager::DefaultMaterialFileSuffix;
    mPlayer->SetSkeletalMeshFilePath((*mSkeletalMeshObjects.begin())->FullResourcePath);
    mPlayer->Load();
    TSingleton<FPlayerController>::GetInstance().SetPlayer(mPlayer);
    TSingleton<FPlayerController>::GetInstance().SetCamera(GetCamera());
}

void UWorld::Unload()
{
    for (auto it = mCameras.begin(); it != mCameras.end(); it++)
    {
        UCamera* camera = *it;
        camera->Unload();
        delete camera;
    }
    mCameras.clear();

    for (auto it = mDirectionalLights.begin(); it != mDirectionalLights.end(); it++)
    {
        UDirectionalLight* directionalLight = *it;
        directionalLight->Unload();
        delete directionalLight;
    }
    mDirectionalLights.clear();

    for (auto it = mStaticMeshObjects.begin(); it != mStaticMeshObjects.end(); it++)
    {
        UStaticMeshObject* staticMeshObject = *it;
        staticMeshObject->Unload();
        delete staticMeshObject;
    }
    mStaticMeshObjects.clear();

    for (auto it = mSkeletalMeshObjects.begin(); it != mSkeletalMeshObjects.end(); it++)
    {
        USkeletalMeshObject* skeletalMeshObject = *it;
        skeletalMeshObject->Unload();
        delete skeletalMeshObject;
    }
    mSkeletalMeshObjects.clear();

    mPlayer->Unload();
    delete mPlayer;
    mPlayer = nullptr;
}

void UWorld::Update(float deltaSeconds)
{
    GetCamera()->Update();

    for (auto it = mSkeletalMeshObjects.begin(); it != mSkeletalMeshObjects.end(); it++)
    {
        USkeletalMeshObject* skeletalMeshObject = *it;
        skeletalMeshObject->Update(deltaSeconds);
    }

    mPlayer->Update(deltaSeconds);
}
