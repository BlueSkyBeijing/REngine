#include "PrecompiledHeader.h"

#include "UCamera.h"
#include "FEngine.h"
#include "FRenderThread.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FView.h"
#include "FRenderCommand.h"

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
    Up = FVector3(0.0f, 0.0f, 1.0f);
    Look = (Target - Position).normalized();
    Right = Up.cross(Look).normalized();

    Update();
}

void UCamera::AdjustMoveStraight(float deltaMove)
{
    Position += Look * deltaMove;
}

void UCamera::AdjustMoveLaterally(float deltaMove)
{
    Position += Right * deltaMove;
}

void UCamera::AdjustPitch(float deltaPitch)
{
    FMatrix3x3 rotationMatrix;
    rotationMatrix = Eigen::AngleAxisf(deltaPitch, Right);

    Up = rotationMatrix * Up;
    Look = rotationMatrix * Look;
}

void UCamera::AdjustYaw(float deltaYaw)
{
    FMatrix3x3 rotationMatrix;
    rotationMatrix = Eigen::AngleAxisf(deltaYaw, FVector3(0.0f, 0.0f, 1.0f));

    Right = rotationMatrix * Right;
    Look = rotationMatrix * Look;
}

void UCamera::Update()
{
    FView* view = new FView();
    view->AspectRatio = AspectRatio;
    view->FOV = FOV;
    view->Look = Look;
    view->Position = Position;
    view->Right = Right;
    view->Target = Target;
    view->Up = Up;

    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();

    ENQUEUE_RENDER_COMMAND([renderThread, view]
    {
        renderThread->SetView(view);
    });

}
