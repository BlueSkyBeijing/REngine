﻿#include "PrecompiledHeader.h"

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
#include "FConsoleVariableManager.h"



UWorld::UWorld(FEngine* engine) :
    mEngine(engine)
{
    std::function <void(std::string&)> printHistory = [](std::string& fileName)
    {
        UWorld* world = TSingleton<FEngine>::GetInstance().GetWorld();
        world->LoadWorld(fileName);
    };

    FConsoleVariable GLoadMap(std::string("load").c_str(), printHistory);

}

UWorld::~UWorld()
{
}

void UWorld::Load()
{
    loadFromFile(TSingleton<FConfigManager>::GetInstance().DefaultMap);

    //tell render thread load completed
    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();

    ENQUEUE_RENDER_COMMAND([renderThread]
    {
        renderThread->MarkLoadCompleted();
    });
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

    for (auto it = mPointLights.begin(); it != mPointLights.end(); it++)
    {
        UPointLight* pointLight = *it;
        pointLight->Unload();
        delete pointLight;
    }
    mPointLights.clear();

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

void UWorld::LoadWorld(std::string fileName)
{
    Unload();

    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();
    ENQUEUE_RENDER_COMMAND([renderThread]
    {
        renderThread->UnInitScene();
    });

    loadFromFile(fileName);
}

void UWorld::loadFromFile(std::string fileName)
{
    FullFilePathName = TSingleton<FConfigManager>::GetInstance().DefaultMapPath + fileName;

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

    int32 numPointLight;
    std::vector<FPointLightData> pointLightDatas;
    mapFile.read((char*)&numPointLight, sizeof(int32));
    for (int32 i = 0; i < numPointLight; i++)
    {
        FPointLightData data;
        mapFile.read((char*)data.Color.data(), sizeof(FVector4));
        mapFile.read((char*)data.Location.data(), sizeof(FVector3));
        mapFile.read((char*)&data.Intensity, sizeof(float));
        mapFile.read((char*)&data.AttenuationRadius, sizeof(float));
        mapFile.read((char*)&data.LightFalloffExponent, sizeof(float));

        pointLightDatas.push_back(data);
    }

    int32 numStaticMeshObject;
    std::vector<FStaticMeshObjectData> staticMeshObjectDatas;
    mapFile.read((char*)&numStaticMeshObject, sizeof(int32));
    for (int32 i = 0; i < numStaticMeshObject; i++)
    {
        FStaticMeshObjectData data;
        mapFile.read((char*)&data.Rotation, sizeof(FQuat));
        mapFile.read((char*)data.Location.data(), sizeof(FVector3));
        mapFile.read((char*)data.Scale.data(), sizeof(FVector3));
        int32 stringSize;
        mapFile.read((char*)&stringSize, sizeof(int32));
        mapFile.read((char*)data.ResourceName.data(), stringSize);

        int32 numMaterial;
        mapFile.read((char*)&numMaterial, sizeof(int32));
        for (int32 iMat = 0; iMat < numMaterial; iMat++)
        {
            std::string materialName;
            mapFile.read((char*)&stringSize, sizeof(int32));
            mapFile.read((char*)materialName.data(), stringSize);
            data.MaterialNames.push_back(materialName);
        }

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
        mapFile.read((char*)data.Scale.data(), sizeof(FVector3));
        int32 stringSize;
        mapFile.read((char*)&stringSize, sizeof(int32));
        mapFile.read((char*)data.ResourceName.data(), stringSize);
        int32 stringSizeAnim;
        mapFile.read((char*)&stringSizeAnim, sizeof(int32));
        mapFile.read((char*)data.AnimationName.data(), stringSizeAnim);

        int32 numMaterial;
        mapFile.read((char*)&numMaterial, sizeof(int32));
        for (int32 iMat = 0; iMat < numMaterial; iMat++)
        {
            std::string materialName;
            mapFile.read((char*)&stringSize, sizeof(int32));
            mapFile.read((char*)materialName.data(), stringSize);
            data.MaterialNames.push_back(materialName);
        }

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

    mPointLights.resize(numPointLight);
    int32 pointLightDataIndex = 0;
    for (auto it = mPointLights.begin(); it != mPointLights.end(); it++)
    {
        UPointLight* pointLight = new UPointLight();

        pointLight->Color = pointLightDatas[pointLightDataIndex].Color;
        pointLight->Location = pointLightDatas[pointLightDataIndex].Location;
        pointLight->Intensity = pointLightDatas[pointLightDataIndex].Intensity;
        pointLight->AttenuationRadius = pointLightDatas[pointLightDataIndex].AttenuationRadius;
        pointLight->LightFalloffExponent = pointLightDatas[pointLightDataIndex].LightFalloffExponent;

        pointLight->Load();

        *it = pointLight;

        pointLightDataIndex++;
    }


    mStaticMeshObjects.resize(numStaticMeshObject);
    int32 staticMeshObjectDataIndex = 0;
    for (auto it = mStaticMeshObjects.begin(); it != mStaticMeshObjects.end(); it++)
    {
        UStaticMeshObject* staticMeshObject = new UStaticMeshObject();

        staticMeshObject->Position = staticMeshObjectDatas[staticMeshObjectDataIndex].Location;
        staticMeshObject->Rotation = staticMeshObjectDatas[staticMeshObjectDataIndex].Rotation;
        staticMeshObject->Scale = staticMeshObjectDatas[staticMeshObjectDataIndex].Scale;
        staticMeshObject->FullResourcePath = FConfigManager::DefaultStaticMeshPath +
            std::string(staticMeshObjectDatas[staticMeshObjectDataIndex].ResourceName.c_str()) +
            FConfigManager::DefaultStaticMeshFileSuffix;
        staticMeshObject->Name = std::string(staticMeshObjectDatas[staticMeshObjectDataIndex].ResourceName.c_str());
        int32 numMat = int32(staticMeshObjectDatas[staticMeshObjectDataIndex].MaterialNames.size());
        for (int i = 0; i < numMat; i++)
        {
            std::string materialName = FConfigManager::DefaultMaterialPath +
                std::string(staticMeshObjectDatas[staticMeshObjectDataIndex].MaterialNames[i].c_str()) +
                FConfigManager::DefaultMaterialFileSuffix;

            staticMeshObject->FullMaterialPaths.push_back(materialName);
        }

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
        skeletalMeshObject->Scale = skeletalMeshObject[staticMeshObjectDataIndex].Scale;
        skeletalMeshObject->FullResourcePath = FConfigManager::DefaultSkeletalMeshPath +
            std::string(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].ResourceName.c_str()) +
            FConfigManager::DefaultSkeletalMeshFileSuffix;
        skeletalMeshObject->Name = std::string(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].ResourceName.c_str());
        skeletalMeshObject->FullAnimSequencePath = FConfigManager::DefaultAnimSequencePath +
            std::string(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].AnimationName.c_str()) +
            FConfigManager::DefaultAnimSequenceFileSuffix;
        int32 numMat = int32(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].MaterialNames.size());
        for (int i = 0; i < numMat; i++)
        {
            std::string materialName = FConfigManager::DefaultMaterialPath +
                std::string(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].MaterialNames[i].c_str()) +
                FConfigManager::DefaultMaterialFileSuffix;

            skeletalMeshObject->FullMaterialPaths.push_back(materialName);
        }

        skeletalMeshObject->Load();

        *it = skeletalMeshObject;

        skeletalMeshObjectDataIndex++;
    }

    mPlayer = new UPlayer();
    int32 numMat = int32(skeletalMeshObjectDatas[0].MaterialNames.size());
    for (int i = 0; i < numMat; i++)
    {
        std::string materialName = FConfigManager::DefaultMaterialPath +
            std::string(skeletalMeshObjectDatas[0].MaterialNames[i].c_str()) +
            FConfigManager::DefaultMaterialFileSuffix;

        mPlayer->FullMaterialPaths.push_back(materialName);
    }

    mPlayer->SetSkeletalMeshFilePath((*mSkeletalMeshObjects.begin())->FullResourcePath);
    mPlayer->Load();
    TSingleton<FPlayerController>::GetInstance().SetPlayer(mPlayer);
    TSingleton<FPlayerController>::GetInstance().SetCamera(GetCamera());


}
