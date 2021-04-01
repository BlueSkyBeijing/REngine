#include "FDevice.h"
#include "FAdapter.h"
#include "FRenderProxy.h"
#include "FRenderTarget.h"
#include "FPipelineState.h"
#include "FBuffer.h"
#include "FShader.h"
#include "FRootSignature.h"
#include "FTexture.h"
#include "WindowsUtility.h"
#include "DX12Utility.h"
#include "DDSTextureLoader.h"
#include "pix3.h"

#include <d3dx12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>

FDevice::FDevice(Microsoft::WRL::ComPtr <ID3D12Device> inDXDevice):
    mDX12Device(inDXDevice),
    mAdapterParent(nullptr),
    mEventHandle(0),
    mFenceValue(0),
    mDepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
	mBackBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
    mRTVDescriptorSize(0),
    mDSVDescriptorSize(0),
    mCBVSRVVUAVDescriptorSize(0),
    mViewPort(),
    mScissorRect(),
    mRenderTarget(nullptr)
{
}

FDevice::~FDevice()
{
}

void FDevice::Init()
{
    //create command queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    THROW_IF_FAILED(mDX12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mDX12CommandQueue)));

    //creAte fence
    THROW_IF_FAILED(mDX12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mDX12Fence)));

    mFenceValue = 1;

    //create event
    mEventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

    mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	mRTVDescriptorSize = mDX12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDSVDescriptorSize = mDX12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mCBVSRVVUAVDescriptorSize = mDX12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    //create command allocator
    THROW_IF_FAILED(mDX12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mDX12CommandAllocator)));

    //create command list
    THROW_IF_FAILED(mDX12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mDX12CommandAllocator.Get(), mDX12PipleLineState.Get(), IID_PPV_ARGS(&mDX12CommandList)));

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDescPass;
    cbvHeapDescPass.NumDescriptors = 4;
    cbvHeapDescPass.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDescPass.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDescPass.NodeMask = 0;
    THROW_IF_FAILED(mDX12Device->CreateDescriptorHeap(&cbvHeapDescPass,
        IID_PPV_ARGS(&mCBVSRVUAVHeap)));

    mDX12CommandList->Close();

    ExecuteCommandList();
    FlushCommandQueue();
}

void FDevice::UnInit()
{
    if (mDX12Device.Get() != nullptr)
    {
        FlushCommandQueue();
    }
}

void FDevice::BeginCommmandList()
{
    THROW_IF_FAILED(mDX12CommandAllocator->Reset());

    //reset command list
    THROW_IF_FAILED(mDX12CommandList->Reset(mDX12CommandAllocator.Get(), NULL));

    ID3D12DescriptorHeap* descriptorHeaps[] = { mCBVSRVUAVHeap.Get() };
    mDX12CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void FDevice::EndCommmandList()
{
    mDX12CommandList->Close();
}

void FDevice::ExecuteCommandList()
{
    //execute command list
    ID3D12CommandList* CommandLists[] = { mDX12CommandList.Get() };
    mDX12CommandQueue->ExecuteCommandLists(1, CommandLists);

}

void FDevice::FlushCommandQueue()
{
    const UINT64 cmdFence = mFenceValue;
    mDX12CommandQueue->Signal(mDX12Fence.Get(), cmdFence);
    mFenceValue++;

    if (mDX12Fence->GetCompletedValue() < cmdFence)
    {
        mDX12Fence->SetEventOnCompletion(cmdFence, mEventHandle);
        WaitForSingleObject(mEventHandle, INFINITE);
    }
}

void FDevice::BeginDraw()
{
    CD3DX12_RESOURCE_BARRIER resourceBarries = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->GetRenderBuffer().Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mDX12CommandList->ResourceBarrier(1, &resourceBarries);
}

void FDevice::Clear(const FVector4& color)
{
    //clear color and depth stencil
    const float ClearColor[] = { color.x, color.y, color.z, color.w };
    mDX12CommandList->ClearRenderTargetView(mRenderTarget->GetRenderBufferView(), ClearColor, 0, nullptr);
    mDX12CommandList->ClearDepthStencilView(mRenderTarget->GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

}

void FDevice::EndDraw()
{
    CD3DX12_RESOURCE_BARRIER resourceBarries = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->GetRenderBuffer().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mDX12CommandList->ResourceBarrier(1, &resourceBarries);
}

void FDevice::BeginEvent(std::string& eventName)
{
    PIXBeginEvent(mDX12CommandList.Get(), 0xffffffff, eventName.c_str());
}

void FDevice::EndEvent()
{
    PIXEndEvent(mDX12CommandList.Get());
}

void FDevice::SetRenderTarget(FRenderTarget* renderTarget)
{
    mRenderTarget = dynamic_cast<FRenderWindow*>(renderTarget);
    //set render target
    D3D12_CPU_DESCRIPTOR_HANDLE renderBufferView = renderTarget->GetRenderBufferView();
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = renderTarget->GetDepthStencilView();
    mDX12CommandList->OMSetRenderTargets(1, &renderBufferView, true, &depthStencilView);
}

void FDevice::SetViewPort(FViewPort& viewPort)
{
    mDX12CommandList->RSSetViewports(1, &viewPort);
}

void FDevice::SetSetScissor(D3D12_RECT& scissorRect)
{
    mDX12CommandList->RSSetScissorRects(1, &scissorRect);
}

void FDevice::SetPipelineState(FPipelineState* pipelineState)
{
    mDX12CommandList->SetGraphicsRootSignature(pipelineState->RootSignature->mDX12RootSignature.Get());

    mDX12CommandList->SetPipelineState(pipelineState->mDX12PipleLineState.Get());
}

void FDevice::SetPrimitiveTopology(FPrimitiveTopology topology)
{
    mDX12CommandList->IASetPrimitiveTopology(topology);
}

void FDevice::SetVertexBuffer(FVertexBuffer* buffer)
{
    mDX12CommandList->IASetVertexBuffers(0, 1, &buffer->mVertexBufferView);
}

void FDevice::SetIndexBuffer(FIndexBuffer* buffer)
{
    mDX12CommandList->IASetIndexBuffer(&buffer->mIndexBufferView);
}

void FDevice::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, int32 baseVertexLocation, uint32 startInstanceLocation)
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE descriptor(mCBVSRVUAVHeap->GetGPUDescriptorHandleForHeapStart());

    mDX12CommandList->SetGraphicsRootDescriptorTable(0, descriptor);
    descriptor.Offset(1, mCBVSRVVUAVDescriptorSize);
    mDX12CommandList->SetGraphicsRootDescriptorTable(1, descriptor);
    descriptor.Offset(1, mCBVSRVVUAVDescriptorSize);
    mDX12CommandList->SetGraphicsRootDescriptorTable(2, descriptor);
    descriptor.Offset(1, mCBVSRVVUAVDescriptorSize);
    mDX12CommandList->SetGraphicsRootDescriptorTable(3, descriptor);

    mDX12CommandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void FDevice::CreateVertexBuffer(FVertexBuffer* vertexBuffer)
{
    const uint64 vertexCount = vertexBuffer->Vertexes.size();

    //create vertex buffer
    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC vertexResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexCount * sizeof(FStaticMeshVertex));

    THROW_IF_FAILED(mDX12Device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &vertexResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer->mVertexBuffer)));

    const uint32 vbByteSize = static_cast<uint32>(vertexCount * sizeof(FStaticMeshVertex));

    //copy data
    uint8* vertexBufferData;
    vertexBuffer->mVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexBufferData));
    memcpy(vertexBufferData, vertexBuffer->Vertexes.data(), vbByteSize);
    vertexBuffer->mVertexBuffer->Unmap(0, nullptr);

    //vertex buffer view
    vertexBuffer->mVertexBufferView.BufferLocation = vertexBuffer->mVertexBuffer->GetGPUVirtualAddress();
    vertexBuffer->mVertexBufferView.StrideInBytes = sizeof(FStaticMeshVertex);
    vertexBuffer->mVertexBufferView.SizeInBytes = vbByteSize;
}

void FDevice::CreateIndexBuffer(FIndexBuffer* indexBuffer)
{
    const uint64 indexCount = indexBuffer->Indexes.size();

    //create index buffer
    CD3DX12_RESOURCE_DESC indexResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(indexCount * sizeof(std::uint16_t));
    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
    THROW_IF_FAILED(mDX12Device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &indexResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&indexBuffer->mIndexBuffer)));

    const uint32 ibByteSize = static_cast<uint32>(indexCount * sizeof(uint16));

    uint8* indexBufferData;
    indexBuffer->mIndexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&indexBufferData));
    memcpy(indexBufferData, indexBuffer->Indexes.data(), ibByteSize);
    indexBuffer->mIndexBuffer->Unmap(0, nullptr);

    indexBuffer->mIndexBufferView.BufferLocation = indexBuffer->mIndexBuffer->GetGPUVirtualAddress();
    indexBuffer->mIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
    indexBuffer->mIndexBufferView.SizeInBytes = ibByteSize;;
}

void FDevice::CreateShader(FShader* shader)
{
    Microsoft::WRL::ComPtr<ID3DBlob> errors;

#if defined(_DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    D3DCompileFromFile(shader->FilePathName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, shader->EnterPoint.c_str(), shader->Target.c_str(), compileFlags, 0, &shader->mShader, &errors);
    if (errors != nullptr)
    {
        OutputDebugStringA((char*)errors->GetBufferPointer());
    }
}

void FDevice::CreateRootSignature(FRootSignature* rootSignature)
{
    CD3DX12_ROOT_PARAMETER slotRootParameter[4];

    CD3DX12_DESCRIPTOR_RANGE objTexTable;
    objTexTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE objConTable;
    objConTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE passTexTable;
    passTexTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

    CD3DX12_DESCRIPTOR_RANGE passConTable;
    passConTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

    slotRootParameter[0].InitAsDescriptorTable(1, &objTexTable, D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[1].InitAsDescriptorTable(1, &objConTable, D3D12_SHADER_VISIBILITY_ALL);
    slotRootParameter[2].InitAsDescriptorTable(1, &passTexTable, D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[3].InitAsDescriptorTable(1, &passConTable, D3D12_SHADER_VISIBILITY_ALL);

    const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
        0, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(4, slotRootParameter, 1, &linearWrap,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    Microsoft::WRL::ComPtr <ID3DBlob> signature;
    Microsoft::WRL::ComPtr <ID3DBlob> error;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    THROW_IF_FAILED(mDX12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature->mDX12RootSignature)));
}

void FDevice::CreatePipelineState(FPipelineState* pipelineState)
{
    uint32 NumElements = static_cast<uint32>(pipelineState->VertexLayout->Elements.size());

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = { pipelineState->VertexLayout->Elements.data(), NumElements };
    psoDesc.pRootSignature = pipelineState->RootSignature->mDX12RootSignature.Get();
    psoDesc.DepthStencilState.DepthEnable = TRUE;
    psoDesc.DepthStencilState.StencilEnable = TRUE;
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(pipelineState->VertexShader->mShader->GetBufferPointer()),
        pipelineState->VertexShader->mShader->GetBufferSize()
    };
    psoDesc.PS =
    {
        reinterpret_cast<BYTE*>(pipelineState->PixelShader->mShader->GetBufferPointer()),
        pipelineState->PixelShader->mShader->GetBufferSize()
    };

    D3D12_RASTERIZER_DESC RasterizerState;
    RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    RasterizerState.FrontCounterClockwise = FALSE;
    RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    RasterizerState.DepthClipEnable = TRUE;
    RasterizerState.MultisampleEnable = FALSE;
    RasterizerState.AntialiasedLineEnable = FALSE;
    RasterizerState.ForcedSampleCount = 0;
    RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    psoDesc.RasterizerState = RasterizerState;
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = mBackBufferFormat;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = mDepthStencilFormat;
    THROW_IF_FAILED(mDX12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState->mDX12PipleLineState)));

}
void FDevice::CreateTexture(FTexture2D* texture2D)
{
    FlushCommandQueue();

    //reset command list
    THROW_IF_FAILED(mDX12CommandList->Reset(mDX12CommandAllocator.Get(), NULL));

    THROW_IF_FAILED(DirectX::CreateDDSTextureFromFile12(mDX12Device.Get(),
        mDX12CommandList.Get(), texture2D->FilePathName.c_str(),
        mCurTexture, mCurTextureUploadHeap));

    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorObj(mCBVSRVUAVHeap->GetCPUDescriptorHandleForHeapStart(), texture2D->Slot);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mCurTexture->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = mCurTexture->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    mDX12Device->CreateShaderResourceView(mCurTexture.Get(), &srvDesc, descriptorObj);

    mDX12CommandList->Close();

    //execute command list
    ID3D12CommandList* CommandLists[] = { mDX12CommandList.Get() };
    mDX12CommandQueue->ExecuteCommandLists(1, CommandLists);
}

void FDevice::CreateRenderTarget(FRenderWindow* renderTarget)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swapChainDesc.BufferDesc.Width = renderTarget->Width;
    swapChainDesc.BufferDesc.Height = renderTarget->Height;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = renderTarget->Format;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = renderTarget->mSwapChainBufferCount;
    swapChainDesc.OutputWindow = renderTarget->mWindowHandle;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    THROW_IF_FAILED(mDXGIFactory->CreateSwapChain(
        mDX12CommandQueue.Get(),
        &swapChainDesc,
        &renderTarget->mDXGISwapChain));

    //create render view description
    D3D12_DESCRIPTOR_HEAP_DESC rtDescriptorHeapDesc;
    rtDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtDescriptorHeapDesc.NumDescriptors = renderTarget->mSwapChainBufferCount;
    rtDescriptorHeapDesc.NodeMask = 0;
    THROW_IF_FAILED(mDX12Device->CreateDescriptorHeap(&rtDescriptorHeapDesc, IID_PPV_ARGS(&renderTarget->mDX12DescriptorHeapRenderTarget)));

    D3D12_DESCRIPTOR_HEAP_DESC dsDescriptorHeapDesc;
    dsDescriptorHeapDesc.NumDescriptors = 1;
    dsDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsDescriptorHeapDesc.NodeMask = 0;
    THROW_IF_FAILED(mDX12Device->CreateDescriptorHeap(&dsDescriptorHeapDesc, IID_PPV_ARGS(&renderTarget->mDX12DescriptorHeapDepthStencil)));

    //create render view
    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(renderTarget->mDX12DescriptorHeapRenderTarget->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < renderTarget->mSwapChainBufferCount; i++)
    {
        renderTarget->mDXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget->mRenderTargets[i]));
        mDX12Device->CreateRenderTargetView(renderTarget->mRenderTargets[i].Get(), nullptr, descriptorHandle);
        descriptorHandle.Offset(1, mRTVDescriptorSize);
    }

    //create the depth/stencil buffer and view.
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = renderTarget->Width;
    depthStencilDesc.Height = renderTarget->Height;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = mDepthStencilFormat;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = renderTarget->mDepthStencilFormat;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;
    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

    THROW_IF_FAILED(mDX12Device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &clearValue,
        IID_PPV_ARGS(&renderTarget->mDepthStencilBuffer)));

    D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandleDS = CD3DX12_CPU_DESCRIPTOR_HANDLE(
        renderTarget->mDX12DescriptorHeapDepthStencil->GetCPUDescriptorHandleForHeapStart());
    //create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.Texture2D.MipSlice = 0;
    mDX12Device->CreateDepthStencilView(renderTarget->mDepthStencilBuffer.Get(), &dsvDesc, descriptorHandleDS);

    //change the depth state now
    THROW_IF_FAILED(mDX12CommandList->Reset(mDX12CommandAllocator.Get(), nullptr));

    CD3DX12_RESOURCE_BARRIER resourceBarries = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->mDepthStencilBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    mDX12CommandList->ResourceBarrier(1, &resourceBarries);

    THROW_IF_FAILED(mDX12CommandList->Close());

    ID3D12CommandList* cmdsLists[] = { mDX12CommandList.Get() };
    mDX12CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    renderTarget->mRTVDescriptorSize = mRTVDescriptorSize;
    renderTarget->mDSVDescriptorSize = mDSVDescriptorSize;

    FlushCommandQueue();
}

void FDevice::Present()
{
    THROW_IF_FAILED(mRenderTarget->mDXGISwapChain->Present(0, 0));
    
    mRenderTarget->mChainBufferndex = (mRenderTarget->mChainBufferndex + 1) % mRenderTarget->mSwapChainBufferCount;
}
