#include "PrecompiledHeader.h"
#include "UObject.h"

UObject::UObject()
{
}

UObject::~UObject()
{
}

UResource::UResource()
{
}

UResource::~UResource()
{
}

USceneObject::USceneObject():
    Position(),
    Rotation(),
    Scale()
{
}

USceneObject::~USceneObject()
{
}

void USceneObject::Load()
{
}

void USceneObject::Unload()
{
}


UPrimitiveObject::UPrimitiveObject():
    USceneObject()
{
}

UPrimitiveObject::~UPrimitiveObject()
{
}

void UPrimitiveObject::Load()
{
}

void UPrimitiveObject::Unload()
{
}

void UPrimitiveObject::createRenderProxy()
{
}
