#pragma once

#include "UObject.h"
class FEngine;

class UCamera : public USceneObject
{
public:
    UCamera(FEngine* engine);
    virtual ~UCamera() override;

public:
    FVector3 Target;
    FVector3 Up;

public:
    virtual void Load() override;

protected:
    void initView();

private:
    FEngine* mEngine;

};
