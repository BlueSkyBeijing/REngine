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
    mCameraArmLength = 600.0f;
}

void FPlayerController::UnInit()
{
}

void FPlayerController::Update(float deltaSeconds)
{
    //update camera
    FMatrix3x3 rotationMatrix;
    FVector3 axis = mPlayer->Rotation._transformVector(FVector3(1.0f, 0.0f, 0.0f));
    axis.normalize();
    rotationMatrix = Eigen::AngleAxisf(mCameraPitchAngle, FVector3(1.0f, 0.0f, 0.0f));

    FVector3 armDir = mPlayer->Rotation.inverse()._transformVector(rotationMatrix * FVector3(0.0f, 1.0f, 0.0f));
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
