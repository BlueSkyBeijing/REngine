#include "PrecompiledHeader.h"

#include "ULight.h"
#include "FEngine.h"


ULight::ULight()
{
}

ULight::~ULight()
{
}


UDirectionalLight::UDirectionalLight(FEngine* engine):
    mEngine(engine)
{
}

UDirectionalLight::~UDirectionalLight()
{
}