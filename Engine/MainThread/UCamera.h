#pragma once
#include "Prerequisite.h"

#include "UObject.h"


class UCamera : public USceneObject
{
public:
    UCamera();
    virtual ~UCamera() override;

public:
    FVector3 Target;
    FVector3 Up;

public:
    virtual void Load() override;

protected:
    void updateView();

private:

};
