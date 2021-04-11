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

private:

};

struct FViewPort
{
    float TopLeftX;
    float TopLeftY;
    float Width;
    float Height;
    float MinDepth;
    float MaxDepth;
};

struct FRect
{
    int32    Left;
    int32    Top;
    int32    Right;
    int32    Bottom;
};
