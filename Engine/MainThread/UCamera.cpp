#include "PrecompiledHeader.h"

#include "UCamera.h"
#include "FEngine.h"
#include "FRenderThread.h"
#include "FEngine.h"
#include "TSingleton.h"

UCamera::UCamera() :
    USceneObject(),
    Target(),
    Up()
{
}

UCamera::~UCamera()
{
}

void UCamera::Load()
{
    Position = FVector3(-2000.0f, 0.0f, 1000.0f);
    Target = FVector3(0.0f, 0.0f, 500.0f);
    Up = FVector3(0.0f, 0.0f, 1.0f);
    Look = (Target - Position).normalized();
    Right = Up.cross(Look).normalized();

    updateView();
}
void UCamera::SetPitch(float pitch)
{
    Eigen::Matrix3f rotationMatrix;
    rotationMatrix = Eigen::AngleAxisf(pitch, Right);

    Up = rotationMatrix * Up;
    Look = rotationMatrix * Look;
}

void UCamera::SetYaw(float yaw)
{
    Eigen::Matrix3f rotationMatrix;
    rotationMatrix = Eigen::AngleAxisf(yaw, Up);

    Right = rotationMatrix * Right;
    Look = rotationMatrix * Look;
}

void UCamera::updateView()
{
    TSingleton<FEngine>::GetInstance().GetRenderThread()->SetView(Position, Target, Up, Right, Look);
}
