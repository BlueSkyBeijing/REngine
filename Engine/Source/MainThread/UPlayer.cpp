#include "PrecompiledHeader.h"

#include "UPlayer.h"
#include "USkeletalMeshObject.h"
#include "USkeletalMesh.h"
#include "FConfigManager.h"
#include "USkeletalMesh.h"
#include "FRenderProxy.h"
#include "FRenderCommand.h"
#include "UMaterial.h"
#include "TSingleton.h"
#include "FResourceManager.h"


UPlayer::UPlayer(): mRingIndex(0)
{

}

UPlayer::~UPlayer()
{

}


void UPlayer::Load()
{
    Name = FString("Player");
    mStateAnimMap.insert(std::make_pair(EPlayerState::PS_Stand, FString("Tutorial_Idle")));
    mStateAnimMap.insert(std::make_pair(EPlayerState::PS_Walk, FString("Tutorial_Walk_Fwd")));

    mSkeletalMesh = std::dynamic_pointer_cast<USkeletalMesh>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_SkeletalMesh, mSkeletalMeshFilePath));
    mSkeletalMesh->Name = FString("Player");

    int32 numMat = (int32)FullMaterialPaths.size();
    for (int i = 0; i < numMat; i++)
    {
        TSharedPtr<UMaterial> mat = std::dynamic_pointer_cast<UMaterial>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_Material, FullMaterialPaths[i]));

        mMaterials.push_back(mat);
    }

    const FString animFileName0 =  *FConfigManager::DefaultAnimSequencePath + mStateAnimMap[EPlayerState::PS_Stand] + *FConfigManager::DefaultAnimSequenceFileSuffix;
    const FString animFileName1 = *FConfigManager::DefaultAnimSequencePath + mStateAnimMap[EPlayerState::PS_Walk] + *FConfigManager::DefaultAnimSequenceFileSuffix;

    mAnimRing[0] = std::dynamic_pointer_cast<UAnimSequence>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_Animation, animFileName0));
    mAnimRing[0]->SetSkeleton(mSkeletalMesh->GetSkeleton());
    mAnimRing[0]->Name = mStateAnimMap[EPlayerState::PS_Stand];
    mAnimRing[0]->Load();

    mAnimRing[1] = std::dynamic_pointer_cast<UAnimSequence>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_Animation, animFileName1));
    mAnimRing[1]->SetSkeleton(mSkeletalMesh->GetSkeleton());
    mAnimRing[0]->Name = mStateAnimMap[EPlayerState::PS_Walk];
    mAnimRing[1]->Load();

    mAnimSequenceBlender = new FAnimSequenceBlender(mAnimRing[0].get(), mAnimRing[1].get());
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
    mAnimRing[0] = nullptr;

    mAnimRing[1] = nullptr;

    mSkeletalMesh = nullptr;

    mMaterials.clear();


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

    mAnimSequenceBlender->SetAnimSequence(mAnimRing[mSrcState].get(), mAnimRing[mDestState].get());
    mAnimSequenceBlender->Blend(mAnimTimeRing[mSrcState], mAnimWeightRing[mSrcState], mAnimTimeRing[mDestState], mAnimWeightRing[mDestState]);

    FSkeletalMeshRenderProxy* proxy = dynamic_cast<FSkeletalMeshRenderProxy*>(mRenderProxy);
    for (int i = 0; i < proxy->MeshBatchs.size(); i++)
    {
        FSkeletalMeshBatch* meshBatch = dynamic_cast<FSkeletalMeshBatch*>(mRenderProxy->MeshBatchs[i]);
        meshBatch->BoneFinalTransforms = mAnimSequenceBlender->BoneFinalTransforms;
    }

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
    TArray<FMaterial*> materials;
    for (int i = 0; i < mMaterials.size(); i++)
    {
        materials.push_back(mMaterials[i]->Material);
    }
    initializer.Materials = materials;
    initializer.Position = Position;
    initializer.Rotation = Rotation;
    initializer.Scale = Scale;
    initializer.mSections = mSkeletalMesh->GetSections();

    mRenderProxy = new FSkeletalMeshRenderProxy(initializer);
    mRenderProxy->DebugName = Name;

    //add to scene
    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();
    FRenderProxy* renderProxy = mRenderProxy;

    ENQUEUE_RENDER_COMMAND([renderThread, renderProxy]
    {
        renderProxy->CreateRenderResource();
        renderThread->AddToScene(renderProxy);
    });

}
