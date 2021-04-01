#include "UWorld.h"
#include "FEngine.h"
#include "FRenderThread.h"
#include "UStaticMeshObject.h"
#include "UCamera.h"
#include "ULight.h"

UWorld::UWorld(FEngine* engine):
    mEngine(engine),
    mCamera(nullptr)
{
    FilePathName = L"Content\\Map\\Defualt.map";
}

UWorld::~UWorld()
{
}

void UWorld::Load()
{
    //load from file
    UStaticMeshObject* staticMeshObject = new UStaticMeshObject(mEngine);
    staticMeshObject->Load();
    mStaticMeshObjects.push_back(staticMeshObject);

    UCamera* camera = new UCamera(mEngine);
    camera->Load();
    mCamera = camera;

    UDirectionalLight* directionalLight = new UDirectionalLight(mEngine);
    directionalLight->Load();
    mDirectionalLight = directionalLight;

    //tell render thread load completed
    mEngine->GetRenderThread()->MarkLoadCompleted();
}

void UWorld::Unload()
{
    //load from file
    for (auto iter = mStaticMeshObjects.begin(); iter != mStaticMeshObjects.end(); iter++)
    {
        UStaticMeshObject* staticMeshObject = *iter;
        staticMeshObject->Unload();
        delete staticMeshObject;
    }

    mStaticMeshObjects.clear();

    mCamera->Unload();
    delete mCamera;
    mCamera = nullptr;

    mDirectionalLight->Unload();
    delete mDirectionalLight;
    mDirectionalLight = nullptr;
}
