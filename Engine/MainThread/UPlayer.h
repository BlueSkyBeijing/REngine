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
    void SetState(EPlayerState state);

    void Update(float deltaSeconds);

protected:
    virtual void createRenderProxy() override;

private:
    std::string mSkeletalMeshFilePath;
    USkeletalMesh* mSkeletalMesh;
    UMaterial* mMaterial;
    FRenderProxy* mRenderProxy;
    FVector3 mOrientation;

    std::map<EPlayerState, std::string> mStateAnimMap;
    UAnimSequence* mAnimRing[PLAYER_ANIMATION_RING_NUM];
    float mAnimTimeRing[PLAYER_ANIMATION_RING_NUM];
    float mAnimWeightRing[PLAYER_ANIMATION_RING_NUM];
    FAnimSequenceBlender* mAnimSequenceBlender;
    int32 mRingIndex;

    EPlayerState mState;
    EPlayerState mSrcState;
    EPlayerState mDestState;
    float mStateBlendTime;

};
