#include "PrecompiledHeader.h"

#include "UPlayer.h"
#include "USkeletalMeshObject.h"
#include "USkeletalMesh.h"
#include "FConfigManager.h"
#include "USkeletalMesh.h"
#include "FRenderProxy.h"
#include "FRenderCommand.h"
#include "UMaterial.h"

UPlayer::UPlayer(): mRingIndex(0)
{

}

UPlayer::~UPlayer()
{

}


void UPlayer::Load()
{
    Name = "Player";
    mStateAnimMap.insert(std::make_pair(EPlayerState::PS_Stand, std::string("Tutorial_Idle")));
    mStateAnimMap.insert(std::make_pair(EPlayerState::PS_Walk, std::string("Tutorial_Walk_Fwd")));

    mSkeletalMesh = new USkeletalMesh();
    mSkeletalMesh->FullFilePathName = mSkeletalMeshFilePath;
    mSkeletalMesh->Name = "Player";
    mSkeletalMesh->Load();

    mAnimRing[0] = new UAnimSequence(mSkeletalMesh->GetSkeleton());
    mAnimRing[0]->FullFilePathName = FConfigManager::DefaultAnimSequencePath + mStateAnimMap[EPlayerState::PS_Stand] + FConfigManager::DefaultAnimSequenceFileSuffix;
    mAnimRing[0]->Name = mStateAnimMap[EPlayerState::PS_Stand];
    mAnimRing[0]->Load();

    mAnimRing[1] = new UAnimSequence(mSkeletalMesh->GetSkeleton());
    mAnimRing[1]->FullFilePathName = FConfigManager::DefaultAnimSequencePath + mStateAnimMap[EPlayerState::PS_Walk] + FConfigManager::DefaultAnimSequenceFileSuffix;
    mAnimRing[0]->Name = mStateAnimMap[EPlayerState::PS_Walk];
    mAnimRing[1]->Load();

    mMaterial = new UMaterial();
    mMaterial->FullFilePathName = FullMaterialPath;
    mMaterial->Load();

    mAnimSequenceBlender = new FAnimSequenceBlender(mAnimRing[0], mAnimRing[1]);
    mAnimWeightRing[0] = 1.0f;
    mAnimWeightRing[1] = 0.0f;

    mState = EPlayerState::PS_Stand;
    mSrcState = EPlayerState::PS_Stand;
    mDestState = EPlayerState::PS_Stand;

    mOrientation = FVector3(0.0f, 1.0f, 0.0f);
    Position = FVector3(0.0f, 0.0f, 0.0f);
    Rotation = FQuat::Identity();

    //create render proxy
    createRenderProxy();

}

void UPlayer::Unload()
{
    mAnimRing[0]->Unload();
    delete mAnimRing[0];
    mAnimRing[0] = nullptr;

    mAnimRing[1]->Unload();
    delete mAnimRing[1];
    mAnimRing[1] = nullptr;

    mSkeletalMesh->Unload();
    delete mSkeletalMesh;
    mSkeletalMesh = nullptr;

    mMaterial->Unload();
    delete mMaterial;
    mMaterial = nullptr;

    delete mAnimSequenceBlender;
    mAnimSequenceBlender = nullptr;

}

void UPlayer::SetState(EPlayerState state)
{
    mSrcState = mState;
    mState = state;
    mDestState = state;
}

void UPlayer::Update(float deltaSeconds)
{
    //update state
    const float lerpTime = 0.5f;
    if (mSrcState != mDestState)
    {
        mStateBlendTime += deltaSeconds;

        if (mStateBlendTime >= lerpTime)
        {
            mAnimTimeRing[mSrcState] = mAnimTimeRing[mDestState];
            mAnimWeightRing[mSrcState] = mAnimWeightRing[mDestState];

            mAnimTimeRing[mDestState] = 0.0f;
            mAnimWeightRing[mSrcState] = 0.0f;

            mSrcState = mDestState;
        }
    }
    else
    {
        mStateBlendTime = 0.0f;
    }

    for (int32 i = 0; i < PLAYER_ANIMATION_RING_NUM; i++)
    {
        if (mAnimWeightRing[i] > 0.000001f)
        {
            mAnimTimeRing[i] += deltaSeconds;
        }
        else
        {
            mAnimTimeRing[i] = 0.0f;
        }
    }

    mAnimWeightRing[mDestState] = mStateBlendTime / lerpTime;
    mAnimWeightRing[mSrcState] = 1.0f - mAnimWeightRing[mDestState];

    mAnimSequenceBlender->SetAnimSequence(mAnimRing[mSrcState], mAnimRing[mDestState]);
    mAnimSequenceBlender->Blend(mAnimTimeRing[mSrcState], mAnimWeightRing[mSrcState], mAnimTimeRing[mDestState], mAnimWeightRing[mDestState]);

    FSkeletalMeshRenderProxy* proxy = dynamic_cast<FSkeletalMeshRenderProxy*>(mRenderProxy);
    proxy->BoneFinalTransforms = mAnimSequenceBlender->BoneFinalTransforms;
    proxy->Position = Position;
    proxy->Rotation = Rotation;

    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();
    FRenderProxy* renderProxy = mRenderProxy;
    ENQUEUE_RENDER_COMMAND([renderThread, renderProxy]
    {
        renderProxy->UpdateRenderResource();
    });

}

void UPlayer::MoveStraight(float deltaDistance)
{
    Position += mOrientation * deltaDistance;

}

void UPlayer::Turn(float deltaAngle)
{
    FMatrix3x3 rotationMatrix;
    rotationMatrix = Eigen::AngleAxisf(deltaAngle, FVector3(0.0f, 0.0f, 1.0f));

    mOrientation = rotationMatrix.inverse() * mOrientation;
    mOrientation.normalize();

    Rotation = Rotation * rotationMatrix;
    Rotation.normalize();
}

void UPlayer::createRenderProxy()
{
    //new in main thread and release in render thread
    FSkeletalMeshRenderProxyInitializer initializer;
    initializer.VertexLayout = mSkeletalMesh->GetVertexLayout();
    initializer.Vertexes = mSkeletalMesh->GetVertexes();
    initializer.Indexes = mSkeletalMesh->GetIndexes();
    initializer.VertexLayout = mSkeletalMesh->GetVertexLayout();
    initializer.Material = mMaterial->Material;
    initializer.Position = Position;
    initializer.Rotation = Rotation;
    initializer.Scale = Scale;

    mRenderProxy = new FSkeletalMeshRenderProxy(initializer);
    mRenderProxy->DebugName = Name;

    //add to scene
    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();
    FRenderProxy* renderProxy = mRenderProxy;

    ENQUEUE_RENDER_COMMAND([renderThread, renderProxy]
    {
        renderThread->AddToScene(renderProxy);
        renderProxy->CreateRenderResource();
    });

}