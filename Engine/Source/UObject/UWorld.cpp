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
#include "FConsoleVariableManager.h"



UWorld::UWorld(FEngine* engine) :
    mEngine(engine)
{
    std::function <void(FString&)> printHistory = [](FString& fileName)
    {
        UWorld* world = TSingleton<FEngine>::GetInstance().GetWorld();
        world->LoadWorld(fileName);
    };

    FConsoleVariable GLoadMap(FString("load").c_str(), printHistory);

}

UWorld::~UWorld()
{
}

void UWorld::Load()
{
    loadFromFile(*TSingleton<FConfigManager>::GetInstance().DefaultMap);

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
        TSharedPtr<UCamera> camera = *it;
        camera->Unload();
    }
    mCameras.clear();

    for (auto it = mDirectionalLights.begin(); it != mDirectionalLights.end(); it++)
    {
        TSharedPtr<UDirectionalLight> directionalLight = *it;
        directionalLight->Unload();
    }
    mDirectionalLights.clear();

    for (auto it = mPointLights.begin(); it != mPointLights.end(); it++)
    {
        TSharedPtr<UPointLight> pointLight = *it;
        pointLight->Unload();
    }
    mPointLights.clear();

    for (auto it = mStaticMeshObjects.begin(); it != mStaticMeshObjects.end(); it++)
    {
        TSharedPtr<UStaticMeshObject> staticMeshObject = *it;
        staticMeshObject->Unload();
    }
    mStaticMeshObjects.clear();

    for (auto it = mSkeletalMeshObjects.begin(); it != mSkeletalMeshObjects.end(); it++)
    {
        TSharedPtr<USkeletalMeshObject> skeletalMeshObject = *it;
        skeletalMeshObject->Unload();
    }
    mSkeletalMeshObjects.clear();

    if (mPlayer)
    {
        mPlayer->Unload();
        mPlayer = nullptr;
    }
}

void UWorld::Update(float deltaSeconds)
{
    if (TSharedPtr<UCamera> cam = GetCamera())
    {
        cam->Update();
    }

    for (auto it = mSkeletalMeshObjects.begin(); it != mSkeletalMeshObjects.end(); it++)
    {
        TSharedPtr<USkeletalMeshObject> skeletalMeshObject = *it;
        skeletalMeshObject->Update(deltaSeconds);
    }

    if(mPlayer)
    {
        mPlayer->Update(deltaSeconds);
    }
}

void UWorld::LoadWorld(FString fileName)
{
    Unload();

    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();
    ENQUEUE_RENDER_COMMAND([renderThread]
    {
        renderThread->UnInitScene();
    });

    loadFromFile(fileName);
}

void UWorld::loadFromFile(FString fileName)
{
    FullFilePathName = *TSingleton<FConfigManager>::GetInstance().DefaultMapPath + fileName;

    //load from file
    std::ifstream mapFile(FullFilePathName, std::ios::in | std::ios::binary);
    if (!mapFile)
    {
        //print error
        return;
    }

    int32 numCamera;
    TArray<FCameraData> cameraDatas;
    mapFile.read((char*)&numCamera, sizeof(int32));
    cameraDatas.resize(numCamera);
    mapFile.read((char*)cameraDatas.data(), numCamera * sizeof(FCameraData));

    int32 numDirectionalLight;
    TArray<FDirectionalLightData> directionalLightDatas;
    mapFile.read((char*)&numDirectionalLight, sizeof(int32));
    for (int32 i = 0; i < numDirectionalLight; i++)
    {
        FDirectionalLightData data;
        mapFile.read((char*)data.Color.data(), sizeof(FVector4));
        mapFile.read((char*)data.Direction.data(), sizeof(FVector3));
        mapFile.read((char*)&data.Intensity, sizeof(float));
        mapFile.read((char*)&data.ShadowDistance, sizeof(float));
        mapFile.read((char*)&data.ShadowBias, sizeof(float));

        directionalLightDatas.push_back(data);
    }

    int32 numPointLight;
    TArray<FPointLightData> pointLightDatas;
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
    TArray<FStaticMeshObjectData> staticMeshObjectDatas;
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
            FString materialName;
            mapFile.read((char*)&stringSize, sizeof(int32));
            mapFile.read((char*)materialName.data(), stringSize);
            data.MaterialNames.push_back(materialName);
        }

        staticMeshObjectDatas.push_back(data);
    }

    int32 numSkeletalMeshObject;
    TArray<FSkeletalMeshObjectData> skeletalMeshObjectDatas;
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
            FString materialName;
            mapFile.read((char*)&stringSize, sizeof(int32));
            mapFile.read((char*)materialName.data(), stringSize);
            data.MaterialNames.push_back(materialName);
        }

        skeletalMeshObjectDatas.push_back(data);
    }

    mapFile.close();

    //init from import data
    if (numCamera > 0)
    {
        mCameras.resize(numCamera);
        int32 camerDataIndex = 0;
        for (auto it = mCameras.begin(); it != mCameras.end(); it++)
        {
            TSharedPtr<UCamera> camera(new UCamera());

            camera->Position = cameraDatas[camerDataIndex].Position;
            camera->Target = cameraDatas[camerDataIndex].Target;
            camera->FOV = cameraDatas[camerDataIndex].FOV;
            camera->AspectRatio = cameraDatas[camerDataIndex].AspectRatio;

            camera->Load();

            *it = camera;

            camerDataIndex++;
        }
    }
    else
    {
        TSharedPtr<UCamera> camera(new UCamera());

        camera->Position = FVector3(500.0f, 500.0f, 500.0f);
        camera->Target = FVector3(0.0f, 0.0f, 0.0f);
        camera->FOV = 45.0f;
        camera->AspectRatio = 1.77777f;

        camera->Load();

        mCameras.push_back(camera);
    }

    mDirectionalLights.resize(numDirectionalLight);
    int32 directionalLightDataIndex = 0;
    for (auto it = mDirectionalLights.begin(); it != mDirectionalLights.end(); it++)
    {
        TSharedPtr<UDirectionalLight> directionalLight(new UDirectionalLight());

        directionalLight->Color = directionalLightDatas[directionalLightDataIndex].Color;
        directionalLight->Direction = directionalLightDatas[directionalLightDataIndex].Direction;
        directionalLight->Intensity = directionalLightDatas[directionalLightDataIndex].Intensity;
        directionalLight->ShadowDistance = directionalLightDatas[directionalLightDataIndex].ShadowDistance;
        directionalLight->ShadowBias = directionalLightDatas[directionalLightDataIndex].ShadowBias;

        directionalLight->Load();

        *it = directionalLight;

        directionalLightDataIndex++;
    }

    mPointLights.resize(numPointLight);
    int32 pointLightDataIndex = 0;
    for (auto it = mPointLights.begin(); it != mPointLights.end(); it++)
    {
        TSharedPtr<UPointLight> pointLight(new UPointLight());

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
        TSharedPtr<UStaticMeshObject> staticMeshObject(new UStaticMeshObject());

        staticMeshObject->Position = staticMeshObjectDatas[staticMeshObjectDataIndex].Location;
        staticMeshObject->Rotation = staticMeshObjectDatas[staticMeshObjectDataIndex].Rotation;
        staticMeshObject->Scale = staticMeshObjectDatas[staticMeshObjectDataIndex].Scale;
        staticMeshObject->FullResourcePath = *FConfigManager::DefaultStaticMeshPath +
            FString(staticMeshObjectDatas[staticMeshObjectDataIndex].ResourceName.c_str()) +
            *FConfigManager::DefaultStaticMeshFileSuffix;
        staticMeshObject->Name = FString(staticMeshObjectDatas[staticMeshObjectDataIndex].ResourceName.c_str());
        int32 numMat = int32(staticMeshObjectDatas[staticMeshObjectDataIndex].MaterialNames.size());
        for (int i = 0; i < numMat; i++)
        {
            FString materialName = *FConfigManager::DefaultMaterialPath +
                FString(staticMeshObjectDatas[staticMeshObjectDataIndex].MaterialNames[i].c_str()) +
                *FConfigManager::DefaultMaterialFileSuffix;

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
        TSharedPtr<USkeletalMeshObject> skeletalMeshObject(new USkeletalMeshObject());

        skeletalMeshObject->Position = skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].Location;
        skeletalMeshObject->Rotation = skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].Rotation;
        skeletalMeshObject->Scale = skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].Scale;
        skeletalMeshObject->FullResourcePath = *FConfigManager::DefaultSkeletalMeshPath +
            FString(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].ResourceName.c_str()) +
            *FConfigManager::DefaultSkeletalMeshFileSuffix;
        skeletalMeshObject->Name = FString(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].ResourceName.c_str());
        skeletalMeshObject->FullAnimSequencePath = *FConfigManager::DefaultAnimSequencePath +
            FString(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].AnimationName.c_str()) +
            *FConfigManager::DefaultAnimSequenceFileSuffix;
        int32 numMat = int32(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].MaterialNames.size());
        for (int i = 0; i < numMat; i++)
        {
            FString materialName = *FConfigManager::DefaultMaterialPath +
                FString(skeletalMeshObjectDatas[skeletalMeshObjectDataIndex].MaterialNames[i].c_str()) +
                *FConfigManager::DefaultMaterialFileSuffix;

            skeletalMeshObject->FullMaterialPaths.push_back(materialName);
        }

        skeletalMeshObject->Load();

        *it = skeletalMeshObject;

        skeletalMeshObjectDataIndex++;
    }

    if (skeletalMeshObjectDatas.size() > 0)
    {
        mPlayer.reset(new UPlayer());
        int32 numMat = int32(skeletalMeshObjectDatas[0].MaterialNames.size());
        for (int i = 0; i < numMat; i++)
        {
            FString materialName = *FConfigManager::DefaultMaterialPath +
                FString(skeletalMeshObjectDatas[0].MaterialNames[i].c_str()) +
                *FConfigManager::DefaultMaterialFileSuffix;

            mPlayer->FullMaterialPaths.push_back(materialName);
        }

        mPlayer->SetSkeletalMeshFilePath((*mSkeletalMeshObjects.begin())->FullResourcePath);
        mPlayer->Load();
        TSingleton<FPlayerController>::GetInstance().SetPlayer(mPlayer.get());
        TSingleton<FPlayerController>::GetInstance().SetCamera(GetCamera().get());
    }

}
