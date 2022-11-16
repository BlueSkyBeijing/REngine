#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"
#include "UPlayer.h"

class FPlayerController
{
    friend class TSingleton<FPlayerController>;

public:
    void Init();
    void UnInit();

    void Update(float deltaSeconds);

    void EnterMoveState()
    {
        mPlayer->SetState(EPlayerState::PS_Walk);
    }

    void EnterStandState()
    {
        mPlayer->SetState(EPlayerState::PS_Stand);
    }

    void MoveStraight(float deltaDistance);
    void Turn(float deltaAngle);

    void AdjustPitch(float deltaPitch);

    inline void SetPlayer(UPlayer* player)
    {
        mPlayer = player;
    }

    inline void SetCamera(UCamera* camera)
    {
        mCamera = camera;
    }

private:
    FPlayerController();
	~FPlayerController();

private:
    UPlayer* mPlayer;
    UCamera* mCamera;
    float mCameraArmLength;
    float mCameraPitchAngle;
    bool mNeedUpdate;
};
