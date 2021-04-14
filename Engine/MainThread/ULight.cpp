#include "PrecompiledHeader.h"

#include "ULight.h"
#include "FEngine.h"


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
