#pragma once
#include "Prerequisite.h"

class FView
{
public:
    FView();
    ~FView();

public:
    FVector3 Position;
    FVector3 Target;
    FVector3 Up;
    FVector3 Right;
    FVector3 Look;
    float FOV;
    float AspectRatio;

private:

};

struct FRect
{
    int32    Left;
    int32    Top;
    int32    Right;
    int32    Bottom;
};
