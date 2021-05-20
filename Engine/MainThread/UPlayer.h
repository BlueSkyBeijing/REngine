#pragma once
#include "Prerequisite.h"

#include "UObject.h"
#include "USkeletalMesh.h"

#define PLAYER_ANIMATION_RING_NUM 2

enum EPlayerState
{
    PS_Stand,
    PS_Walk
};

class UPlayer : public UPrimitiveObject
{
public:
	UPlayer();
	~UPlayer();

    virtual void Load() override;
    virtual void Unload() override;

    inline void SetSkeletalMeshFilePath(const std::string& object)
    {
        mSkeletalMeshFilePath = object;
    }

    inline const std::string& GetSkeletalMeshFilePath() const
    {
        return mSkeletalMeshFilePath;
    }

    inline FVector3 GetOrientation()
    {
        return mOrientation;
    }

    void MoveStraight(float deltaDistance);
    void Turn(float deltaAngle);

    void Update(float deltaSeconds);
    void SetState(EPlayerState state)
    {
        mSrcState = mState;
        mState = state;
        mDestState = state;
        mAnimTimeRing[mDestState] = 0.0f;
    }

protected:
    virtual void createRenderProxy() override;

private:
    std::string mSkeletalMeshFilePath;
    USkeletalMesh* mSkeletalMesh;
    UMaterial* mMaterial;
    FVector3 mOrientation;
    std::map<EPlayerState, std::string> mStateAnimMap;
    UAnimSequence* mAnimRing[PLAYER_ANIMATION_RING_NUM];
    float mAnimTimeRing[PLAYER_ANIMATION_RING_NUM];
    float mAnimWeightRing[PLAYER_ANIMATION_RING_NUM];
    FAnimSequenceBlender* mAnimSequenceBlender;
    std::vector<FMatrix4x4> BoneFinalTransforms;
    int32 mRingIndex;
    FRenderProxy* mRenderProxy;

    EPlayerState mState;
    EPlayerState mSrcState;
    EPlayerState mDestState;
    float mStateBlendTime;

};
