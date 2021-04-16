#include "PrecompiledHeader.h"

#include "ULight.h"
#include "FEngine.h"
#include "FLight.h"
#include "FRenderThread.h"
#include "TSingleton.h"
#include "FRenderCommand.h"

ULight::ULight() :
    Color(1.0f, 1.0f, 1.0f, 1.0f),
    Intensity(1.0f)
{
}

ULight::~ULight()
{
}


UDirectionalLight::UDirectionalLight() :
    ULight(),
    Direction(-1.0f, -1.0f, -1.0f)
{
}

UDirectionalLight::~UDirectionalLight()
{
}

void UDirectionalLight::Load()
{
    FDirectionalLight* light = new FDirectionalLight;
    light->Color = Color;
    light->Direction = Direction;
    light->Intensity = Intensity;

    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();

    ENQUEUE_RENDER_COMMAND([renderThread, light]
    {
        renderThread->SetDirectionalLight(light);
    });
}
