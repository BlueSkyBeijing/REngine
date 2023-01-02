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
    light->ShadowDistance = ShadowDistance;
    light->ShadowBias = ShadowBias;

    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();

    ENQUEUE_RENDER_COMMAND([renderThread, light]
    {
        renderThread->AddLight(light);
    });
}

UPointLight::UPointLight() :
    ULight()
{
}

UPointLight::~UPointLight()
{
}

void UPointLight::Load()
{
    FPointLight* light = new FPointLight;
    light->Color = Color;
    light->Location = Location;
    light->Intensity = Intensity;
    light->AttenuationRadius = AttenuationRadius;
    light->LightFalloffExponent = LightFalloffExponent;

    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();

    ENQUEUE_RENDER_COMMAND([renderThread, light]
    {
        renderThread->AddLight(light);
    });
}
