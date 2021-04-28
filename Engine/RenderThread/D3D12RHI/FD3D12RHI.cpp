#include "PrecompiledHeader.h"

#include "FD3D12RHI.h"
#include "FRenderProxy.h"
#include "FD3D12RenderTarget.h"
#include "FD3D12PipelineState.h"
#include "FD3D12Buffer.h"
#include "FD3D12Shader.h"
#include "FD3D12ShaderBindings.h"
#include "FD3D12Texture.h"
#include "WindowsUtility.h"
#include "DX12Utility.h"
#include "DDSTextureLoader.h"
#include "FEngine.h"
#include "TSingleton.h"


int32 FD3D12RHI::msObjectSRVTableIndex = 0;
int32 FD3D12RHI::msObjectCBVTableIndex = 1;
int32 FD3D12RHI::msPassSRVTableIndex = 2;
int32 FD3D12RHI::msPassCBVTableIndex = 3;

int32 FD3D12RHI::msRTVCount = 0;
int32 FD3D12RHI::msDSVCount = 0;
int32 FD3D12RHI::msCBVSRVUAVCount = 0;

FD3D12RHI::FD3D12RHI() :
    mDX12Device(nullptr),
    mEventHandle(0),
    mFenceValue(0),
    mDepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
    mBackBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
    mRTVDescriptorSize(0),
    mDSVDescriptorSize(0),
    mCBVSRVVUAVDescriptorSize(0),
    mViewPort(),
    mScissorRect(),
    mRenderTargetCurrent(nullptr)
{
}

FD3D12RHI::~FD3D12RHI()
{
}

void FD3D12RHI::Init()
{
#if defined(DEBUG) || defined(_DEBUG) 
    //enable DX12 debug layer
    {
        Microsoft::WRL::ComPtr <ID3D12Debug> debugController;
        D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
        debugController->EnableDebugLayer();
    }
#endif

    Microsoft::WRL::ComPtr <IDXGIAdapter1> adapter;
    Microsoft::WRL::ComPtr <ID3D12Device> dx12Device;

    Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
    {
        THROW_IF_FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&mDXGIFactory)));

        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
    }
    else
    {
        THROW_IF_FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory)));
    }

    //create DX12 device
    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != mDXGIFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc = {};
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }

        HRESULT Result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dx12Device));
        if (SUCCEEDED(Result))
        {
            mDX12Device = dx12Device;
            break;
        }
    }

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

    createHeaps();

    mDX12CommandList->Close();

    ExecuteCommandList();
    FlushCommandQueue();
}

void FD3D12RHI::UnInit()
{
    if (mDX12Device.Get() != nullptr)
    {
        FlushCommandQueue();
    }
}

void FD3D12RHI::BeginCommmandList()
{
    THROW_IF_FAILED(mDX12CommandAllocator->Reset());

    //reset command list
    THROW_IF_FAILED(mDX12CommandList->Reset(mDX12CommandAllocator.Get(), NULL));

    ID3D12DescriptorHeap* descriptorHeaps[] = { mCBVSRVUAVHeap.Get() };
    mDX12CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void FD3D12RHI::EndCommmandList()
{
    mDX12CommandList->Close();
}

void FD3D12RHI::ExecuteCommandList()
{
    //execute command list
    ID3D12CommandList* CommandLists[] = { mDX12CommandList.Get() };
    mDX12CommandQueue->ExecuteCommandLists(1, CommandLists);

}

void FD3D12RHI::FlushCommandQueue()
{
    const uint64 cmdFence = mFenceValue;
    mDX12CommandQueue->Signal(mDX12Fence.Get(), cmdFence);
    mFenceValue++;

    if (mDX12Fence->GetCompletedValue() < cmdFence)
    {
        mDX12Fence->SetEventOnCompletion(cmdFence, mEventHandle);
        WaitForSingleObject(mEventHandle, INFINITE);
    }
}

void FD3D12RHI::Clear(bool clearColor, const FVector4& color, bool clearDepth, float depth, bool clearStencil, uint32 stencil)
{
    //clear color and depth stencil
    if (clearColor)
    {
        const float ClearColor[] = { color.x(), color.y(), color.z(), color.w() };
        FD3D12RenderTarget* renderTargetDX12 = dynamic_cast<FD3D12RenderTarget*>(mRenderTargetCurrent);
        FD3D12RenderWindow* renderWindowDX12 = dynamic_cast<FD3D12RenderWindow*>(mRenderTargetCurrent);

        if (renderTargetDX12 != nullptr)
        {
            mDX12CommandList->ClearRenderTargetView(renderTargetDX12->GetRenderTargetView(renderTargetDX12->GetRenderTargetIndex()), ClearColor, 0, nullptr);
        }
        else if (renderWindowDX12 != nullptr)
        {
            mDX12CommandList->ClearRenderTargetView(renderWindowDX12->GetRenderTargetView(renderWindowDX12->GetRenderTargetIndex()), ClearColor, 0, nullptr);
        }
    }

    uint32 clearFlags = 0;
    if (clearDepth)
    {
        clearFlags |= D3D12_CLEAR_FLAG_DEPTH;
    }

    if (clearStencil)
    {
        clearFlags |= D3D12_CLEAR_FLAG_STENCIL;
    }

    if (clearFlags != 0)
    {
        FD3D12RenderTarget* renderTargetDX12 = dynamic_cast<FD3D12RenderTarget*>(mRenderTargetCurrent);
        FD3D12RenderWindow* renderWindowDX12 = dynamic_cast<FD3D12RenderWindow*>(mRenderTargetCurrent);

        if (renderTargetDX12 != nullptr)
        {
            mDX12CommandList->ClearDepthStencilView(renderTargetDX12->GetDepthStencilView(), (D3D12_CLEAR_FLAGS)clearFlags, depth, stencil, 0, nullptr);
        }
        else if (renderWindowDX12 != nullptr)
        {
            mDX12CommandList->ClearDepthStencilView(renderWindowDX12->GetDepthStencilView(), (D3D12_CLEAR_FLAGS)clearFlags, depth, stencil, 0, nullptr);
        }

    }

}

void FD3D12RHI::BeginEvent(const char* eventName)
{
    PIXBeginEvent(mDX12CommandList.Get(), 0xffffffff, eventName);
}

void FD3D12RHI::EndEvent()
{
    PIXEndEvent(mDX12CommandList.Get());
}

void FD3D12RHI::BeginRenderPass(const FRHIRenderPassInfo& info, const char* name)
{

}

void FD3D12RHI::EndRenderPass()
{

}

void FD3D12RHI::SetRenderTarget(FRHIRenderTarget* renderTarget)
{
    mRenderTargetCurrent = renderTarget;
    FD3D12RenderTarget* renderTargetDX12 = dynamic_cast<FD3D12RenderTarget*>(renderTarget);
    FD3D12RenderWindow* renderWindowDX12 = dynamic_cast<FD3D12RenderWindow*>(renderTarget);

    if (renderTargetDX12 != nullptr)
    {
        if (renderTargetDX12->mRenderTargetFormat == DXGI_FORMAT_UNKNOWN)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = renderTargetDX12->GetDepthStencilView();

            mDX12CommandList->OMSetRenderTargets(0, nullptr, false, &depthStencilView);
        }
        else
        {
            D3D12_CPU_DESCRIPTOR_HANDLE renderBufferView = renderTargetDX12->GetRenderTargetView(renderTargetDX12->GetRenderTargetIndex());
            D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = renderTargetDX12->GetDepthStencilView();

            mDX12CommandList->OMSetRenderTargets(1, &renderBufferView, true, &depthStencilView);
        }

    }
    else if (renderWindowDX12 != nullptr)
    {
        if (renderWindowDX12->mRenderTargetFormat == DXGI_FORMAT_UNKNOWN)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = renderWindowDX12->GetDepthStencilView();

            mDX12CommandList->OMSetRenderTargets(0, nullptr, false, &depthStencilView);
        }
        else
        {
            D3D12_CPU_DESCRIPTOR_HANDLE renderBufferView = renderWindowDX12->GetRenderTargetView(renderWindowDX12->GetRenderTargetIndex());
            D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = renderWindowDX12->GetDepthStencilView();

            mDX12CommandList->OMSetRenderTargets(1, &renderBufferView, true, &depthStencilView);
        }
    }
}

void FD3D12RHI::SetViewPort(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
    const D3D12_VIEWPORT viewport = { minX, minY, (maxX - minX), (maxY - minY), minZ, maxZ };
    mDX12CommandList->RSSetViewports(1, &viewport);
}

void FD3D12RHI::SetSetScissor(bool enable, float minX, float minY, float maxX, float maxY)
{
    const D3D12_RECT rect = { static_cast<LONG>(minX), static_cast<LONG>(minY), static_cast<LONG>(maxX), static_cast<LONG>(maxY) };
    mDX12CommandList->RSSetScissorRects(1, &rect);
}

void FD3D12RHI::SetPipelineState(FRHIPipelineState* pipelineState)
{
    FD3D12PipelineState* state = dynamic_cast<FD3D12PipelineState*>(pipelineState);
    FD3D12ShaderBindings* bindings = dynamic_cast<FD3D12ShaderBindings*>(state->ShaderBindings);

    mDX12CommandList->SetGraphicsRootSignature(bindings->mDX12RootSignature.Get());

    mDX12CommandList->SetPipelineState(state->mDX12PipleLineState.Get());
}

void FD3D12RHI::SetPrimitiveType(EPrimitiveType primitiveType)
{
    D3D_PRIMITIVE_TOPOLOGY dxToplogy = translatePrimitiveType(primitiveType);
    mDX12CommandList->IASetPrimitiveTopology(dxToplogy);
}

void FD3D12RHI::SetVertexBuffer(FRHIVertexBuffer* buffer)
{
    FD3D12VertexBuffer* vertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(buffer);

    mDX12CommandList->IASetVertexBuffers(0, 1, &vertexBuffer->mVertexBufferView);
}

void FD3D12RHI::SetIndexBuffer(FRHIIndexBuffer* buffer)
{
    FD3D12IndexBuffer* indexBuffer = dynamic_cast<FD3D12IndexBuffer*>(buffer);

    mDX12CommandList->IASetIndexBuffer(&indexBuffer->mIndexBufferView);
}

void FD3D12RHI::SetConstantBuffer(FRHIConstantBuffer* buffer, int32 shaderPos)
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorObject(mCBVSRVUAVHeap->GetGPUDescriptorHandleForHeapStart());

    descriptorObject.Offset(buffer->PosInHeap, mCBVSRVVUAVDescriptorSize);
    mDX12CommandList->SetGraphicsRootDescriptorTable(shaderPos == 0 ? msObjectCBVTableIndex : msPassCBVTableIndex, descriptorObject);

}

void FD3D12RHI::SetTexture2D(FRHITexture2D* texture, int32 shaderPos)
{
    FD3D12RenderTarget* renderTargetDX12 = dynamic_cast<FD3D12RenderTarget*>(texture);
    int32 posInHeap = texture->PosInHeap;
    if (renderTargetDX12 != nullptr)
    {
        if (renderTargetDX12->mRenderTargets.size() > 0)
        {
            posInHeap = renderTargetDX12->PosInHeapRTSRV;
        }
        else
        {
            posInHeap = renderTargetDX12->PosInHeapDSSRV;
        }
    }
    CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorObject(mCBVSRVUAVHeap->GetGPUDescriptorHandleForHeapStart());

    descriptorObject.Offset(posInHeap, mCBVSRVVUAVDescriptorSize);
    mDX12CommandList->SetGraphicsRootDescriptorTable(shaderPos == 0 ? msObjectSRVTableIndex : msPassSRVTableIndex, descriptorObject);
}

void FD3D12RHI::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, int32 baseVertexLocation, uint32 startInstanceLocation)
{
    mDX12CommandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

FRHIVertexBuffer* FD3D12RHI::CreateVertexBuffer(uint32 structureSize, uint32 vertexCount, uint8* bufferData)
{
    FD3D12VertexBuffer* vertexBuffer = new FD3D12VertexBuffer;

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

    const uint32 vbByteSize = static_cast<uint32>(vertexCount * structureSize);

    //copy data
    uint8* vertexBufferData;
    vertexBuffer->mVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexBufferData));
    memcpy(vertexBufferData, bufferData, vbByteSize);
    vertexBuffer->mVertexBuffer->Unmap(0, nullptr);

    //vertex buffer view
    vertexBuffer->mVertexBufferView.BufferLocation = vertexBuffer->mVertexBuffer->GetGPUVirtualAddress();
    vertexBuffer->mVertexBufferView.StrideInBytes = structureSize;
    vertexBuffer->mVertexBufferView.SizeInBytes = vbByteSize;

    return vertexBuffer;
}

FRHIIndexBuffer* FD3D12RHI::CreateIndexBuffer(uint32 structureSize, uint32 indexCount, uint8* bufferData)
{
    FD3D12IndexBuffer* indexBuffer = new FD3D12IndexBuffer;

    //create index buffer
    CD3DX12_RESOURCE_DESC indexResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(indexCount * structureSize);
    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
    THROW_IF_FAILED(mDX12Device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &indexResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&indexBuffer->mIndexBuffer)));

    const uint32 ibByteSize = static_cast<uint32>(indexCount * structureSize);

    uint8* indexBufferData;
    indexBuffer->mIndexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&indexBufferData));
    memcpy(indexBufferData, bufferData, ibByteSize);
    indexBuffer->mIndexBuffer->Unmap(0, nullptr);

    indexBuffer->mIndexBufferView.BufferLocation = indexBuffer->mIndexBuffer->GetGPUVirtualAddress();
    indexBuffer->mIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
    indexBuffer->mIndexBufferView.SizeInBytes = ibByteSize;;

    return indexBuffer;
}

FRHIConstantBuffer* FD3D12RHI::CreateConstantBuffer(uint32 structureSize, uint8* bufferData)
{
    FD3D12ConstantBuffer* constantBuffer = new FD3D12ConstantBuffer;

    //create object constant buffer
    CD3DX12_RESOURCE_DESC objConstantResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(CalcConstantBufferByteSize(structureSize));

    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
    THROW_IF_FAILED(mDX12Device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &objConstantResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBuffer->mConstantBuffer)));

    uint8* objectConstantBufferData;
    constantBuffer->mConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&objectConstantBufferData));
    memcpy(objectConstantBufferData, bufferData, structureSize);
    constantBuffer->mConstantBuffer->Unmap(0, nullptr);

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDescObject;
    cbvDescObject.BufferLocation = constantBuffer->mConstantBuffer->GetGPUVirtualAddress();
    cbvDescObject.SizeInBytes = CalcConstantBufferByteSize(structureSize);

    CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDescriptor(mCBVSRVUAVHeap->GetCPUDescriptorHandleForHeapStart());

    constantBuffer->PosInHeap = msCBVSRVUAVCount;
    cbvDescriptor = cbvDescriptor.Offset(constantBuffer->PosInHeap, mCBVSRVVUAVDescriptorSize);

    msCBVSRVUAVCount++;

    mDX12Device->CreateConstantBufferView(
        &cbvDescObject,
        cbvDescriptor);

    return constantBuffer;
}

FRHIShader* FD3D12RHI::CreateShader(const std::wstring& filePathName, const std::string& enterPoint, const std::string& target)
{
    FD3D12Shader* shader = new FD3D12Shader;

    Microsoft::WRL::ComPtr<ID3DBlob> errors;

#if defined(_DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    D3DCompileFromFile(filePathName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, enterPoint.c_str(), target.c_str(), compileFlags, 0, &shader->mShader, &errors);
    if (errors != nullptr)
    {
        OutputDebugStringA((char*)errors->GetBufferPointer());
    }

    return shader;
}

FRHIShaderBindings* FD3D12RHI::CreateShaderBindings()
{
    FD3D12ShaderBindings* rootSignature = new FD3D12ShaderBindings;

    CD3DX12_ROOT_PARAMETER slotRootParameter[4];

    CD3DX12_DESCRIPTOR_RANGE objTexTable;
    objTexTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE objConTable;
    objConTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE passTexTable;
    passTexTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

    CD3DX12_DESCRIPTOR_RANGE passConTable;
    passConTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

    slotRootParameter[msObjectSRVTableIndex].InitAsDescriptorTable(1, &objTexTable, D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[msObjectCBVTableIndex].InitAsDescriptorTable(1, &objConTable, D3D12_SHADER_VISIBILITY_ALL);
    slotRootParameter[msPassSRVTableIndex].InitAsDescriptorTable(1, &passTexTable, D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[msPassCBVTableIndex].InitAsDescriptorTable(1, &passConTable, D3D12_SHADER_VISIBILITY_ALL);

    const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
        0, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC shadow(
        1, // shaderRegister
        D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
        0.0f,                               // mipLODBias
        16,                                 // maxAnisotropy
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> sampler = { linearWrap , shadow };

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(4, slotRootParameter, 2, sampler.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    Microsoft::WRL::ComPtr <ID3DBlob> signature;
    Microsoft::WRL::ComPtr <ID3DBlob> error;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    THROW_IF_FAILED(mDX12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature->mDX12RootSignature)));

    return rootSignature;
}

FRHIPipelineState* FD3D12RHI::CreatePipelineState(FRHIShaderBindings* shaderBindings, FRHIShader* vertexShader, FRHIShader* pixelShader, FRHIVertexLayout* vertexLayout)
{
    FD3D12PipelineState* pipelineState = new FD3D12PipelineState;
    pipelineState->ShaderBindings = shaderBindings;
    FD3D12Shader* vs = dynamic_cast<FD3D12Shader*>(vertexShader);
    FD3D12Shader* ps = dynamic_cast<FD3D12Shader*>(pixelShader);

    uint32 numElements = static_cast<uint32>(vertexLayout->Elements.size());

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = { (D3D12_INPUT_ELEMENT_DESC*)vertexLayout->Elements.data(), numElements };
    psoDesc.pRootSignature = dynamic_cast<FD3D12ShaderBindings*>(shaderBindings)->mDX12RootSignature.Get();
    psoDesc.DepthStencilState.DepthEnable = TRUE;
    psoDesc.DepthStencilState.StencilEnable = TRUE;
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(vs->mShader->GetBufferPointer()),
        vs->mShader->GetBufferSize()
    };
    psoDesc.PS =
    {
        reinterpret_cast<BYTE*>(ps->mShader->GetBufferPointer()),
        ps->mShader->GetBufferSize()
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

    return pipelineState;

}

FRHIPipelineState* FD3D12RHI::CreatePipelineStateFullScreenQuad(FRHIShaderBindings* shaderBindings, FRHIShader* vertexShader, FRHIShader* pixelShader, FRHIVertexLayout* vertexLayout)
{
    FD3D12PipelineState* pipelineState = new FD3D12PipelineState;
    pipelineState->ShaderBindings = shaderBindings;
    FD3D12Shader* vs = dynamic_cast<FD3D12Shader*>(vertexShader);
    FD3D12Shader* ps = dynamic_cast<FD3D12Shader*>(pixelShader);

    uint32 numElements = static_cast<uint32>(vertexLayout->Elements.size());

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = { (D3D12_INPUT_ELEMENT_DESC*)vertexLayout->Elements.data(), numElements };
    psoDesc.pRootSignature = dynamic_cast<FD3D12ShaderBindings*>(shaderBindings)->mDX12RootSignature.Get();
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(vs->mShader->GetBufferPointer()),
        vs->mShader->GetBufferSize()
    };
    psoDesc.PS =
    {
        reinterpret_cast<BYTE*>(ps->mShader->GetBufferPointer()),
        ps->mShader->GetBufferSize()
    };

    D3D12_RASTERIZER_DESC RasterizerState;
    RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    RasterizerState.FrontCounterClockwise = FALSE;
    RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    RasterizerState.DepthClipEnable = FALSE;
    RasterizerState.MultisampleEnable = FALSE;
    RasterizerState.AntialiasedLineEnable = FALSE;
    RasterizerState.ForcedSampleCount = 0;
    RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    psoDesc.RasterizerState = RasterizerState;
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = mBackBufferFormat;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = mDepthStencilFormat;
    THROW_IF_FAILED(mDX12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState->mDX12PipleLineState)));

    return pipelineState;

}

FRHIPipelineState* FD3D12RHI::CreatePipelineStateShadow(FRHIShaderBindings* shaderBindings, FRHIShader* vertexShader, FRHIShader* pixelShader, FRHIVertexLayout* vertexLayout)
{
    FD3D12PipelineState* pipelineState = new FD3D12PipelineState;
    pipelineState->ShaderBindings = shaderBindings;
    FD3D12Shader* vs = dynamic_cast<FD3D12Shader*>(vertexShader);
    FD3D12Shader* ps = dynamic_cast<FD3D12Shader*>(pixelShader);

    uint32 numElements = static_cast<uint32>(vertexLayout->Elements.size());

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = { (D3D12_INPUT_ELEMENT_DESC*)vertexLayout->Elements.data(), numElements };
    psoDesc.pRootSignature = dynamic_cast<FD3D12ShaderBindings*>(shaderBindings)->mDX12RootSignature.Get();
    psoDesc.DepthStencilState.DepthEnable = TRUE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(vs->mShader->GetBufferPointer()),
        vs->mShader->GetBufferSize()
    };

    if (ps != nullptr)
    {
        psoDesc.PS =
        {
            reinterpret_cast<BYTE*>(ps->mShader->GetBufferPointer()),
            ps->mShader->GetBufferSize()
        };
    }

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
    psoDesc.RasterizerState.DepthBias = 2;
    psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
    psoDesc.RasterizerState.SlopeScaledDepthBias = 2.0f;
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = mDepthStencilFormat;
    psoDesc.NumRenderTargets = 0;
    THROW_IF_FAILED(mDX12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState->mDX12PipleLineState)));

    return pipelineState;

}

FRHITexture2D* FD3D12RHI::CreateTexture2D(const std::wstring& filePathName)
{
    FD3D12Texture2D* texture2D = new FD3D12Texture2D;

    FlushCommandQueue();

    //reset command list
    THROW_IF_FAILED(mDX12CommandList->Reset(mDX12CommandAllocator.Get(), NULL));

    THROW_IF_FAILED(DirectX::CreateDDSTextureFromFile12(mDX12Device.Get(),
        mDX12CommandList.Get(), filePathName.c_str(),
        mCurTexture, mCurTextureUploadHeap));


    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorObj(mCBVSRVUAVHeap->GetCPUDescriptorHandleForHeapStart(), msCBVSRVUAVCount, mCBVSRVVUAVDescriptorSize);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mCurTexture->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = mCurTexture->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    mDX12Device->CreateShaderResourceView(mCurTexture.Get(), &srvDesc, descriptorObj);

    texture2D->PosInHeap = msCBVSRVUAVCount;

    msCBVSRVUAVCount++;

    mDX12CommandList->Close();

    //execute command list
    ID3D12CommandList* CommandLists[] = { mDX12CommandList.Get() };
    mDX12CommandQueue->ExecuteCommandLists(1, CommandLists);

    return texture2D;
}


FRHIRenderTarget* FD3D12RHI::CreateRenderTarget(uint32 width, uint32 hight, uint32 numTarget, EPixelFormat formatTarget, EPixelFormat formatDepthStencil)
{
    FD3D12RenderTarget* renderTarget = new FD3D12RenderTarget(width, hight, numTarget);
    renderTarget->mDX12DescriptorHeapDepthStencil = mDX12DescriptorHeapDepthStencil;
    renderTarget->mDX12DescriptorHeapRenderTarget = mDX12DescriptorHeapRenderTarget;
    renderTarget->mDSVDescriptorSize = mDSVDescriptorSize;
    renderTarget->mRTVDescriptorSize = mRTVDescriptorSize;
    renderTarget->mRenderTargetFormat = DXGI_FORMAT_UNKNOWN;
    renderTarget->PosInHeap = msRTVCount;
    renderTarget->Init();

    if (formatTarget != PF_UNKNOWN)
    {
        for (uint32 i = 0; i < numTarget; i++)
        {
            //create render view
            CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(mDX12DescriptorHeapRenderTarget->GetCPUDescriptorHandleForHeapStart(), msRTVCount, mRTVDescriptorSize);
            D3D12_RESOURCE_DESC renderTargetDesc;
            renderTargetDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            renderTargetDesc.Alignment = 0;
            renderTargetDesc.Width = renderTarget->Width;
            renderTargetDesc.Height = renderTarget->Height;
            renderTargetDesc.DepthOrArraySize = 1;
            renderTargetDesc.MipLevels = 1;
            renderTargetDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            renderTargetDesc.SampleDesc.Count = 1;
            renderTargetDesc.SampleDesc.Quality = 0;
            renderTargetDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            renderTargetDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            renderTarget->mRenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

            if (i == 0)
            {
                renderTarget->PosInHeap = msRTVCount;
            }

            msRTVCount++;

            D3D12_CLEAR_VALUE clearValue;
            clearValue.Color[0] = 0.5f;
            clearValue.Color[1] = 0.5f;
            clearValue.Color[2] = 0.5f;
            clearValue.Color[3] = 1.0f;
            clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

            THROW_IF_FAILED(mDX12Device->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &renderTargetDesc,
                D3D12_RESOURCE_STATE_COMMON,
                &clearValue,
                IID_PPV_ARGS(&renderTarget->mRenderTargets[i])));

            mDX12Device->CreateRenderTargetView(renderTarget->mRenderTargets[i].Get(), nullptr, descriptorHandle);

            CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorObj(mCBVSRVUAVHeap->GetCPUDescriptorHandleForHeapStart(), msCBVSRVUAVCount, mCBVSRVVUAVDescriptorSize);

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = renderTarget->mRenderTargets[i]->GetDesc().Format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = renderTarget->mRenderTargets[i]->GetDesc().MipLevels;
            srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

            renderTarget->PosInHeapRTSRV = msCBVSRVUAVCount;
            mDX12Device->CreateShaderResourceView(renderTarget->mRenderTargets[i].Get(), &srvDesc, descriptorObj);
            msCBVSRVUAVCount++;

        }
    }

    //if (formatDepthStencil == PF_R24_UNORM_X8_TYPELESS)
    {
        //create the depth/stencil buffer and view.
        D3D12_RESOURCE_DESC depthStencilDesc;
        depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthStencilDesc.Alignment = 0;
        depthStencilDesc.Width = renderTarget->Width;
        depthStencilDesc.Height = renderTarget->Height;
        depthStencilDesc.DepthOrArraySize = 1;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.Format = (formatDepthStencil == PF_R24_UNORM_X8_TYPELESS) ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE clearValue;
        clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;
        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

        THROW_IF_FAILED(mDX12Device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &depthStencilDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            &clearValue,
            IID_PPV_ARGS(&renderTarget->mDepthStencilBuffer)));

        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandleDS = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDX12DescriptorHeapDepthStencil->GetCPUDescriptorHandleForHeapStart(), msDSVCount, mDSVDescriptorSize);
        //create descriptor to mip level 0 of entire resource using the format of the resource.
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.Texture2D.MipSlice = 0;
        mDX12Device->CreateDepthStencilView(renderTarget->mDepthStencilBuffer.Get(), &dsvDesc, descriptorHandleDS);
        renderTarget->PosInHeapDSV = msDSVCount;
        msDSVCount++;

        CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorObj(mCBVSRVUAVHeap->GetCPUDescriptorHandleForHeapStart(), msCBVSRVUAVCount, mCBVSRVVUAVDescriptorSize);
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
        srvDesc.Texture2D.PlaneSlice = 0;
        mDX12Device->CreateShaderResourceView(renderTarget->mDepthStencilBuffer.Get(), &srvDesc, descriptorObj);
        renderTarget->PosInHeapDSSRV = msCBVSRVUAVCount;

        msCBVSRVUAVCount++;
    }

    renderTarget->mRTVDescriptorSize = mRTVDescriptorSize;
    renderTarget->mDSVDescriptorSize = mDSVDescriptorSize;

    return renderTarget;
}

FRHIRenderWindow* FD3D12RHI::CreateRenderWindow(uint32 width, uint32 hight)
{
    FD3D12RenderWindow* renderTarget = new FD3D12RenderWindow(width, hight, DXGI_FORMAT_R8G8B8A8_UNORM, FRAME_BUFFER_NUM);
    renderTarget->mDX12DescriptorHeapDepthStencil = mDX12DescriptorHeapDepthStencil;
    renderTarget->mDX12DescriptorHeapRenderTarget = mDX12DescriptorHeapRenderTarget;
    renderTarget->mDSVDescriptorSize = mDSVDescriptorSize;
    renderTarget->mRTVDescriptorSize = mRTVDescriptorSize;

    renderTarget->Init();

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swapChainDesc.BufferDesc.Width = renderTarget->Width;
    swapChainDesc.BufferDesc.Height = renderTarget->Height;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = renderTarget->NumTarget;
    swapChainDesc.OutputWindow = TSingleton<FEngine>::GetInstance().GetWindowHandle();
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    THROW_IF_FAILED(mDXGIFactory->CreateSwapChain(
        mDX12CommandQueue.Get(),
        &swapChainDesc,
        &renderTarget->mDXGISwapChain));

    //create render view
    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(renderTarget->mDX12DescriptorHeapRenderTarget->GetCPUDescriptorHandleForHeapStart(), msRTVCount, mRTVDescriptorSize);
    renderTarget->PosInHeap = msRTVCount;

    for (UINT i = 0; i < renderTarget->NumTarget; i++)
    {
        renderTarget->mDXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget->mRenderTargets[i]));
        mDX12Device->CreateRenderTargetView(renderTarget->mRenderTargets[i].Get(), nullptr, descriptorHandle);
        descriptorHandle.Offset(1, mRTVDescriptorSize);
        msRTVCount++;
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
        renderTarget->mDX12DescriptorHeapDepthStencil->GetCPUDescriptorHandleForHeapStart(), msDSVCount, mDSVDescriptorSize);
    //create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.Texture2D.MipSlice = 0;
    mDX12Device->CreateDepthStencilView(renderTarget->mDepthStencilBuffer.Get(), &dsvDesc, descriptorHandleDS);
    renderTarget->PosInHeapDSV = msDSVCount;
    msDSVCount++;
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

    return renderTarget;
}

void FD3D12RHI::UpdateConstantBuffer(FRHIConstantBuffer* constantBuffer, uint32 structureSize, uint8* bufferData)
{
    FD3D12ConstantBuffer* dx12ConstantBuffer = dynamic_cast<FD3D12ConstantBuffer*>(constantBuffer);

    uint8* objectConstantBufferData;
    dx12ConstantBuffer->mConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&objectConstantBufferData));
    memcpy(objectConstantBufferData, bufferData, structureSize);
    dx12ConstantBuffer->mConstantBuffer->Unmap(0, nullptr);
}

void FD3D12RHI::Transition(const FRHITransitionInfo& info)
{
    FD3D12RenderTarget* renderTargetDX12 = dynamic_cast<FD3D12RenderTarget*>(mRenderTargetCurrent);
    FD3D12RenderWindow* renderWindowDX12 = dynamic_cast<FD3D12RenderWindow*>(mRenderTargetCurrent);

    if (renderTargetDX12 != nullptr)
    {
        if ((info.AccessBefore == ACCESS_GENERIC_READ) && (info.AccessAfter == ACCESS_DEPTH_WRITE))
        {
            CD3DX12_RESOURCE_BARRIER resourceBarries = CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDX12->GetDepthStencilBuffer().Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
            mDX12CommandList->ResourceBarrier(1, &resourceBarries);
        }
        else if ((info.AccessBefore == ACCESS_DEPTH_WRITE) && (info.AccessAfter == ACCESS_GENERIC_READ))
        {
            CD3DX12_RESOURCE_BARRIER resourceBarries = CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDX12->GetDepthStencilBuffer().Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
            mDX12CommandList->ResourceBarrier(1, &resourceBarries);
        }
        else if ((info.AccessBefore == ACCESS_PRESENT) && (info.AccessAfter == ACCESS_RENDER_TARGET))
        {
            CD3DX12_RESOURCE_BARRIER resourceBarries = CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDX12->GetRenderTargetBuffer(renderTargetDX12->GetRenderTargetIndex()).Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            mDX12CommandList->ResourceBarrier(1, &resourceBarries);
            CD3DX12_RESOURCE_BARRIER resourceBarriesDS = CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDX12->GetDepthStencilBuffer().Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
            mDX12CommandList->ResourceBarrier(1, &resourceBarriesDS);
        }
        else if ((info.AccessBefore == ACCESS_RENDER_TARGET) && (info.AccessAfter == ACCESS_PRESENT))
        {
            CD3DX12_RESOURCE_BARRIER resourceBarries = CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDX12->GetRenderTargetBuffer(renderTargetDX12->GetRenderTargetIndex()).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            mDX12CommandList->ResourceBarrier(1, &resourceBarries);
            CD3DX12_RESOURCE_BARRIER resourceBarriesDS = CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDX12->GetDepthStencilBuffer().Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
            mDX12CommandList->ResourceBarrier(1, &resourceBarriesDS);
        }
    }
    else if (renderWindowDX12 != nullptr)
    {
        if ((info.AccessBefore == ACCESS_PRESENT) && (info.AccessAfter == ACCESS_RENDER_TARGET))
        {
            CD3DX12_RESOURCE_BARRIER resourceBarries = CD3DX12_RESOURCE_BARRIER::Transition(renderWindowDX12->GetRenderTargetBuffer(renderWindowDX12->GetRenderTargetIndex()).Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            mDX12CommandList->ResourceBarrier(1, &resourceBarries);
        }
        else if ((info.AccessBefore == ACCESS_RENDER_TARGET) && (info.AccessAfter == ACCESS_PRESENT))
        {
            CD3DX12_RESOURCE_BARRIER resourceBarries = CD3DX12_RESOURCE_BARRIER::Transition(renderWindowDX12->GetRenderTargetBuffer(renderWindowDX12->GetRenderTargetIndex()).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            mDX12CommandList->ResourceBarrier(1, &resourceBarries);
        }
    }

}

void FD3D12RHI::Present(FRHIRenderWindow* window)
{
    window->Present();
}

void FD3D12RHI::createHeaps()
{
    //this should be tweaked for each title as heaps require VRAM. The default value of 512k takes up ~16MB
    // D3D12 is guaranteed to support 1M (D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_1) descriptors in a CBV/SRV/UAV heap, so clamp the size to this.
    // https://docs.microsoft.com/en-us/windows/desktop/direct3d12/hardware-support
    const int32 globalSRVCBVUAVHeapSize = 500 * 1000;
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDescPass;
    cbvHeapDescPass.NumDescriptors = globalSRVCBVUAVHeapSize;
    cbvHeapDescPass.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDescPass.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDescPass.NodeMask = 0;
    THROW_IF_FAILED(mDX12Device->CreateDescriptorHeap(&cbvHeapDescPass, IID_PPV_ARGS(&mCBVSRVUAVHeap)));

    D3D12_DESCRIPTOR_HEAP_DESC rtDescriptorHeapDesc;
    rtDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtDescriptorHeapDesc.NumDescriptors = 64;
    rtDescriptorHeapDesc.NodeMask = 0;
    THROW_IF_FAILED(mDX12Device->CreateDescriptorHeap(&rtDescriptorHeapDesc, IID_PPV_ARGS(&mDX12DescriptorHeapRenderTarget)));

    D3D12_DESCRIPTOR_HEAP_DESC dsDescriptorHeapDesc;
    dsDescriptorHeapDesc.NumDescriptors = 64;
    dsDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsDescriptorHeapDesc.NodeMask = 0;
    THROW_IF_FAILED(mDX12Device->CreateDescriptorHeap(&dsDescriptorHeapDesc, IID_PPV_ARGS(&mDX12DescriptorHeapDepthStencil)));
}

D3D_PRIMITIVE_TOPOLOGY FD3D12RHI::translatePrimitiveType(EPrimitiveType primitiveType)
{
    switch (primitiveType)
    {
    case PT_TriangleList:               return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PT_TriangleStrip:              return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case PT_LineList:                   return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    case PT_PointList:                  return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    }

    return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
}
