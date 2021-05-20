#include "PrecompiledHeader.h"

#include "FPlayerController.h"
#include "UPlayer.h"
#include "UCamera.h"

FPlayerController::FPlayerController()
{
}

FPlayerController::~FPlayerController()
{
}

void FPlayerController::Init()
{
    mCameraPitchAngle = 45.0f * 180.0f / 3.1415926f;
    mCameraArmLength = 500.0f;
}

void FPlayerController::UnInit()
{
}

void FPlayerController::Update(float deltaSeconds)
{
    //update camera
    FMatrix3x3 rotationMatrix;
    FVector3 axis = mPlayer->GetOrientation().cross(FVector3(0.0f, 0.0f, 1.0f));
    rotationMatrix = Eigen::AngleAxisf(mCameraPitchAngle, axis);

    FVector3 armDir = -rotationMatrix * mPlayer->GetOrientation();

    mCamera->Position = mPlayer->Position - armDir * mCameraArmLength;
    mCamera->Target = mPlayer->Position;
    mCamera->Look = armDir;
}


void FPlayerController::MoveStraight(float deltaDistance)
{
    mPlayer->MoveStraight(deltaDistance);
}

void FPlayerController::Turn(float deltaAngle)
{
    mPlayer->Turn(deltaAngle);
}
