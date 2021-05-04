#pragma once
#include "Prerequisite.h"

// main thread serializable

class UObject
{
public:
    UObject();
    virtual ~UObject();

public:
    std::string Name;

public:
    virtual void Load() {};
    virtual void Unload() {};
private:

};

class UResource : public UObject
{
public:
    UResource();
    virtual ~UResource();

public:
    std::string FullFilePathName;

public:
    virtual void Load() {};
    virtual void Unload() {};
private:

};

class USceneObject : public UObject
{
public:
    USceneObject();
    virtual ~USceneObject() override;

    virtual void Load() override;
    virtual void Unload() override;

public:
    FVector3 Position;
    FQuat Rotation;
    FVector3 Scale;

protected:

};

class UPrimitiveObject : public USceneObject
{
public:
    UPrimitiveObject();
    virtual~UPrimitiveObject() override;

    virtual void Load() override;
    virtual void Unload() override;

protected:
    virtual void createRenderProxy();

};
