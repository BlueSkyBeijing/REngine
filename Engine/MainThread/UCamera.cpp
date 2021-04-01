#include "UCamera.h"
#include "FEngine.h"
#include "FRenderThread.h"

UCamera::UCamera(FEngine* engine):
    USceneObject(),
    mEngine(engine),
    Target(),
    Up()
{
}

UCamera::~UCamera()
{
}

void UCamera::Load()
{
   Position = FVector3(-2000.0f, 0.0f, 1000.0f);
   Target = FVector3(0.0f, 0.0f, 500.0f);
   Up = FVector3(0.0f, 0.0f, 1.0f);

   initView();
}

void UCamera::initView()
{
    mEngine->GetRenderThread()->InitView(Position, Target, Up);
}
