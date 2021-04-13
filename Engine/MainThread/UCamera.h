﻿#pragma once
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
    FVector3 Right;
    FVector3 Look;

public:
    virtual void Load() override;

    void AdjustMoveStraight(float deltaMove);
    void AdjustMoveLaterally(float deltaMove);

    void AdjustPitch(float deltaPitch);
    void AdjustYaw(float deltaYaw);

protected:
    void updateView();

private:

};
