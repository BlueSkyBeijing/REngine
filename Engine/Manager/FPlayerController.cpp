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
    mCameraPitchAngle = -30.0f * 3.1415926f / 180.0f;
    mCameraArmLength = 800.0f;
}

void FPlayerController::UnInit()
{
}

void FPlayerController::Update(float deltaSeconds)
{
    //update camera
    FMatrix3x3 rotationMatrix;
    FVector3 axis = mPlayer->GetOrientation().cross(FVector3(0.0f, 0.0f, 1.0f));
    axis.normalize();
    rotationMatrix = Eigen::AngleAxisf(mCameraPitchAngle, axis);

    FVector3 armDir = rotationMatrix * mPlayer->GetOrientation();
    armDir.normalize();
    FVector3 target = mPlayer->Position;
    target.z() = 100.0f;

    mCamera->Position = target - armDir * mCameraArmLength;
    mCamera->Target = target;
    mCamera->Look = armDir;
    mCamera->Up = FVector3(0.0f, 0.0f, 1.0f);
    mCamera->Right = mCamera->Up.cross(mCamera->Look).normalized();

}

void FPlayerController::MoveStraight(float deltaDistance)
{
    mPlayer->MoveStraight(deltaDistance);
}

void FPlayerController::Turn(float deltaAngle)
{
    mPlayer->Turn(deltaAngle);
}

void FPlayerController::AdjustPitch(float deltaPitch)
{
    mCameraPitchAngle += deltaPitch;
}
