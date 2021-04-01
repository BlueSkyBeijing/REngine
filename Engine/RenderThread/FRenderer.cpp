#include "FRenderer.h"
#include "FRenderTarget.h"
#include "FRenderProxy.h"
#include "FScene.h"
#include "FDeviceManager.h"
#include "FDevice.h"
#include "FPipelineStateManager.h"
#include "FRootSignatureManager.h"
#include "FShader.h"
#include "Utility.h"
#include "FPipelineState.h"
#include "FRootSignature.h"
#include "TSingleton.h"
#include "FMaterial.h"
#include "FBuffer.h"
#include "FView.h"

#include <vector>
#include <string>

FRenderer::FRenderer(FRenderWindow* renderWindow, FScene* scene, FView* view) :
    mDevice(TSingleton<FDeviceManager>::GetInstance().GetRootDevice()),
    mRenderWindow(renderWindow),
    mScene(scene),
    mView(view)
{
}

FRenderer::~FRenderer()
{
}

void FRenderer::Init()
{
    mDevice->FlushCommandQueue();
}

void FRenderer::UnInit()
{
    mDevice->FlushCommandQueue();

    mPassConstantBuffer->UnInit();
    delete mPassConstantBuffer;
    mPassConstantBuffer = nullptr;
}

void FRenderer::CreateRenderResources()
{
    createPassConstantBuffer();

    std::vector<FRenderProxy*>& renderProxys = mScene->GetRenderProxys();
    for (auto it = renderProxys.begin(); it != renderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;
        renderProxy->CreateRenderResource();
    }

    mDevice->FlushCommandQueue();
}

void FRenderer::preRender()
{
    mDevice->BeginCommmandList();
    mDevice->SetRenderTarget(mRenderWindow);
    mDevice->BeginDraw();
}

void FRenderer::RenderOneFrame()
{
    preRender();

    clear();

    initView();

    drawRenderables();

    postProcess();

    postRender();
}

void FRenderer::clear()
{
    const DirectX::XMFLOAT4 clearColor = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mDevice->Clear(clearColor);
}

void FRenderer::initView()
{
    setViewPort();
    computeVisibility();
}

void FRenderer::computeVisibility()
{
}

void FRenderer::setRenderTarget(FRenderTarget* renderTarget)
{
    mDevice->SetRenderTarget(renderTarget);
}

void FRenderer::setViewPort()
{
    //viewport
    FViewPort viewPort = { 0.0f, 0.0f, static_cast<float>(mRenderWindow->Width), static_cast<float>(mRenderWindow->Height), 0.0f, 1.0f };
    //scissor rectangle
    FRect scissorRect = { 0, 0, static_cast<LONG>(mRenderWindow->Width), static_cast<LONG>(mRenderWindow->Height) };

    mDevice->SetViewPort(viewPort);
    mDevice->SetSetScissor(scissorRect);
}

void FRenderer::drawRenderables()
{
    FDevice* curDevice = mDevice;

    const std::vector<FRenderProxy*>& renderProxys = mScene->GetRenderProxys();
    for (auto it = renderProxys.begin(); it != renderProxys.end(); it++)
    {
        FRenderProxy * renderProxy = *it;

        curDevice->BeginEvent(renderProxy->DebugName);

        FPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetOrCreatePipleLineState(renderProxy);

        curDevice->SetPipelineState(pipelineState);
        curDevice->SetPrimitiveTopology(FPrimitiveTopology::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        curDevice->SetVertexBuffer(renderProxy->VertexBuffer);
        curDevice->SetIndexBuffer(renderProxy->IndexBuffer);
        curDevice->DrawIndexedInstanced(renderProxy->IndexCountPerInstance, renderProxy->InstanceCount, renderProxy->StartIndexLocation, renderProxy->BaseVertexLocation, renderProxy->StartInstanceLocation);

        curDevice->EndEvent();
    }
}

void FRenderer::postProcess()
{
}

void FRenderer::postRender()
{
    mDevice->EndDraw();
    mDevice->EndCommmandList();

    mDevice->ExecuteCommandList();
    mDevice->FlushCommandQueue();
}

void FRenderer::createPassConstantBuffer()
{
    //create pass constant buffer
    FPassConstant constant;
    FMatrix4x4 mViewMatrix;
    FMatrix4x4 mProjMatrix;
    
    //build view matrix.
    DirectX::XMVECTOR pos = DirectX::XMVectorSet(mView->Position.x, mView->Position.y, mView->Position.z, 1.0f);
    DirectX::XMVECTOR target = DirectX::XMVectorSet(mView->Target.x, mView->Target.y, mView->Target.z, 1.0f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(mView->Up.x, mView->Up.y, mView->Up.z, 1.0f);

    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
    DirectX::XMStoreFloat4x4(&mViewMatrix, view);

    const float AspectRatio = 1.7f;
    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, AspectRatio, 1.0f, 100000.0f);
    DirectX::XMStoreFloat4x4(&mProjMatrix, P);

    DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProjMatrix);

    DirectX::XMStoreFloat4x4(&constant.View, DirectX::XMMatrixTranspose(view));
    DirectX::XMStoreFloat4x4(&constant.ViewProj, DirectX::XMMatrixTranspose(view * proj));

    mPassConstantBuffer = new FConstantBuffer<FPassConstant>;
    mPassConstantBuffer->BufferStruct = constant;
    mPassConstantBuffer->Slot = 3;
    mPassConstantBuffer->Init();
}
