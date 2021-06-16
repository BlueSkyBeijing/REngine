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
    mFenceValue(),
    mFrameIndex(0),
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
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
    THROW_IF_FAILED(mDX12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mDX12CommandQueue)));

    //create fence
    THROW_IF_FAILED(mDX12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mDX12Fence)));

    for (uint32 i = 0; i < FRAME_BUFFER_NUM; i++)
    {
        mFenceValue[i] = 1;
    }

    //create event
    mEventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

    mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    mRTVDescriptorSize = mDX12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDSVDescriptorSize = mDX12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mCBVSRVVUAVDescriptorSize = mDX12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    //create command allocator
    THROW_IF_FAILED(mDX12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mDX12CommandAllocator[mFrameIndex])));

    //create command list
    THROW_IF_FAILED(mDX12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mDX12CommandAllocator[mFrameIndex].Get(), mDX12PipleLineState.Get(), IID_PPV_ARGS(&mDX12CommandList)));

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
    THROW_IF_FAILED(mDX12CommandAllocator[mFrameIndex]->Reset());

    //reset command list
    THROW_IF_FAILED(mDX12CommandList->Reset(mDX12CommandAllocator[mFrameIndex].Get(), NULL));

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
    const uint64 cmdFence = mFenceValue[mFrameIndex];

    mDX12CommandQueue->Signal(mDX12Fence.Get(), cmdFence);

    mFenceValue[mFrameIndex]++;

    if (mDX12Fence->GetCompletedValue() < cmdFence)
    {
        mDX12Fence->SetEventOnCompletion(cmdFence, mEventHandle);

        WaitForSingleObject(mEventHandle, INFINITE);
    }
}

void FD3D12RHI::Clear(bool clearColor, const FVector4& color, bool clearDepth, float depth, bool clearStencil, uint32 stencil)
{
    FD3D12RenderTarget* renderTargetDX12 = dynamic_cast<FD3D12RenderTarget*>(mRenderTargetCurrent);

    //clear color and depth stencil
    if (clearColor)
    {
        const float ClearColor[] = { color.x(), color.y(), color.z(), color.w() };

        FD3D12Texture* ti2 = dynamic_cast<FD3D12Texture*>(renderTargetDX12->RenderTargets[renderTargetDX12->GetRenderTargetIndex()]);

        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandleDS2 = CD3DX12_CPU_DESCRIPTOR_HANDLE(
            mDX12DescriptorHeapRenderTarget->GetCPUDescriptorHandleForHeapStart(), ti2->PosInHeapRTVDSV, mRTVDescriptorSize);

        mDX12CommandList->ClearRenderTargetView(descriptorHandleDS2, ClearColor, 0, nullptr);
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
        FD3D12Texture* depthStencil = dynamic_cast<FD3D12Texture*>(renderTargetDX12->DepthStencilTarget);

        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandleDS = CD3DX12_CPU_DESCRIPTOR_HANDLE(
            mDX12DescriptorHeapDepthStencil->GetCPUDescriptorHandleForHeapStart(), depthStencil->PosInHeapRTVDSV, mDSVDescriptorSize);

        mDX12CommandList->ClearDepthStencilView(descriptorHandleDS, (D3D12_CLEAR_FLAGS)clearFlags, depth, stencil, 0, nullptr);
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

    D3D12_CPU_DESCRIPTOR_HANDLE* rtView = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE* dsView = nullptr;

    const int32 numRT = renderTarget->RenderTargets.size() > 0 ? 1 : 0;

    if (renderTarget->RenderTargets.size() > 0)
    {
        FD3D12Texture* renderTexutre = dynamic_cast<FD3D12Texture*>(renderTarget->RenderTargets[renderTarget->GetRenderTargetIndex()]);

        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandleDS = CD3DX12_CPU_DESCRIPTOR_HANDLE(
            mDX12DescriptorHeapRenderTarget->GetCPUDescriptorHandleForHeapStart(), renderTexutre->PosInHeapRTVDSV, mRTVDescriptorSize);

        rtView = &descriptorHandleDS;
    }

    if (renderTarget->DepthStencilTarget != nullptr)
    {
        FD3D12Texture* depthStencil = dynamic_cast<FD3D12Texture*>(renderTarget->DepthStencilTarget);

        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandleDS = CD3DX12_CPU_DESCRIPTOR_HANDLE(
            mDX12DescriptorHeapDepthStencil->GetCPUDescriptorHandleForHeapStart(), depthStencil->PosInHeapRTVDSV, mDSVDescriptorSize);

        dsView = &descriptorHandleDS;
    }

    mDX12CommandList->OMSetRenderTargets(numRT, rtView, numRT > 0, dsView);
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

    FD3D12Resource* resourceDX12 = dynamic_cast<FD3D12Resource*>(buffer);

    descriptorObject.Offset(resourceDX12->PosInHeapCBVSRVUAV, mCBVSRVVUAVDescriptorSize);
    mDX12CommandList->SetGraphicsRootDescriptorTable(shaderPos == 0 ? msObjectCBVTableIndex : msPassCBVTableIndex, descriptorObject);
}

void FD3D12RHI::SetTexture2D(FRHITexture2D* texture, int32 shaderPos)
{
    FD3D12Texture2D* textureDX12 = dynamic_cast<FD3D12Texture2D*>(texture);
    CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorObject(mCBVSRVUAVHeap->GetGPUDescriptorHandleForHeapStart());

    descriptorObject.Offset(textureDX12->PosInHeapCBVSRVUAV, mCBVSRVVUAVDescriptorSize);
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
    CD3DX12_RESOURCE_DESC vertexResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexCount * structureSize);

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

    constantBuffer->PosInHeapCBVSRVUAV = msCBVSRVUAVCount;
    cbvDescriptor = cbvDescriptor.Offset(constantBuffer->PosInHeapCBVSRVUAV, mCBVSRVVUAVDescriptorSize);

    msCBVSRVUAVCount++;

    mDX12Device->CreateConstantBufferView(
        &cbvDescObject,
        cbvDescriptor);

    return constantBuffer;
}

FRHIShader* FD3D12RHI::CreateShader(const FShaderInfo& shaderInfo)
{
    FD3D12Shader* shader = new FD3D12Shader;

    Microsoft::WRL::ComPtr<ID3DBlob> errors;

#if defined(_DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif
    D3D_SHADER_MACRO* shaderDefines = nullptr;
    if (shaderInfo.Defines.size() > 0)
    {
        shaderDefines = new D3D_SHADER_MACRO[shaderInfo.Defines.size() + 1];
        
        std::map<std::string, std::string>::const_iterator it = shaderInfo.Defines.begin();
        int32 index = 0;
        for (; it != shaderInfo.Defines.end(); it++)
        {
            D3D_SHADER_MACRO define;
            define.Name = it->first.c_str();
            define.Definition = it->second.c_str();
            shaderDefines[index] = define;
        }

        D3D_SHADER_MACRO defineEnd;
        defineEnd.Name = NULL;
        defineEnd.Definition = NULL;

        shaderDefines[index + 1] = defineEnd;
    }

    D3DCompileFromFile(shaderInfo.FilePathName.c_str(), shaderDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderInfo.EnterPoint.c_str(), shaderInfo.Target.c_str(), compileFlags, 0, &shader->mShader, &errors);
    if (errors != nullptr)
    {
        OutputDebugStringA((char*)errors->GetBufferPointer());
    }

    shader->FullFilePathName = shaderInfo.FilePathName;
    shader->EnterPoint = shaderInfo.EnterPoint;
    shader->Target = shaderInfo.Target;

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

    const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
        2, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
        3, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 4> sampler = { linearWrap , shadow, linearClamp, pointClamp };

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(4, slotRootParameter, 4, sampler.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    Microsoft::WRL::ComPtr <ID3DBlob> signature;
    Microsoft::WRL::ComPtr <ID3DBlob> error;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    THROW_IF_FAILED(mDX12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature->mDX12RootSignature)));

    return rootSignature;
}

FRHIPipelineState* FD3D12RHI::CreatePipelineState(const FPipelineStateInfo& info)
{
    FD3D12PipelineState* pipelineState = new FD3D12PipelineState;
    pipelineState->ShaderBindings = info.ShaderBindings;
    FD3D12Shader* vs = dynamic_cast<FD3D12Shader*>(info.VertexShader);
    FD3D12Shader* ps = dynamic_cast<FD3D12Shader*>(info.PixelShader);

    uint32 numElements = static_cast<uint32>(info.VertexLayout->Elements.size());

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = { (D3D12_INPUT_ELEMENT_DESC*)info.VertexLayout->Elements.data(), numElements };
    psoDesc.pRootSignature = dynamic_cast<FD3D12ShaderBindings*>(info.ShaderBindings)->mDX12RootSignature.Get();

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
    RasterizerState.DepthClipEnable = info.DepthStencilState.bEnableDepthWrite;
    RasterizerState.MultisampleEnable = FALSE;
    RasterizerState.AntialiasedLineEnable = FALSE;
    RasterizerState.ForcedSampleCount = 0;
    RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    psoDesc.RasterizerState = RasterizerState;
    if (ps == nullptr)
    {
        psoDesc.RasterizerState.DepthBias = 2;
        psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
        psoDesc.RasterizerState.SlopeScaledDepthBias = 2.0f;
    }

    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = info.DepthStencilState.DepthTest != CF_Always;
    psoDesc.DepthStencilState.StencilEnable = info.DepthStencilState.bEnableFrontFaceStencil || info.DepthStencilState.bEnableBackFaceStencil;
    psoDesc.DepthStencilState.DepthWriteMask = info.DepthStencilState.bEnableDepthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;

    D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;

    renderTargetBlendDesc.BlendEnable =
        info.BlendState.ColorBlendOp != BO_Add || info.BlendState.ColorDestBlend != BF_Zero || info.BlendState.ColorSrcBlend != BF_One ||
        info.BlendState.AlphaBlendOp != BO_Add || info.BlendState.AlphaDestBlend != BF_Zero || info.BlendState.AlphaSrcBlend != BF_One;

    renderTargetBlendDesc.BlendOp = translateBlendOp(info.BlendState.ColorBlendOp);
    renderTargetBlendDesc.SrcBlend = translateBlendFactor(info.BlendState.ColorSrcBlend);
    renderTargetBlendDesc.DestBlend = translateBlendFactor(info.BlendState.ColorDestBlend);
    renderTargetBlendDesc.BlendOpAlpha = translateBlendOp(info.BlendState.AlphaBlendOp);
    renderTargetBlendDesc.SrcBlendAlpha = translateBlendFactor(info.BlendState.AlphaSrcBlend);
    renderTargetBlendDesc.DestBlendAlpha = translateBlendFactor(info.BlendState.AlphaDestBlend);
    renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    renderTargetBlendDesc.LogicOpEnable = false;
    psoDesc.BlendState.RenderTarget[0] = renderTargetBlendDesc;

    psoDesc.DepthStencilState.DepthFunc = translateCompareFunction(info.DepthStencilState.DepthTest);
    psoDesc.RasterizerState.CullMode = translateCullMode(info.RasterizerState.CullMode);

    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = (ps == nullptr) ? 0 : 1;
    psoDesc.RTVFormats[0] = (ps == nullptr) ? DXGI_FORMAT_UNKNOWN : translatePixelFormat(info.RenderTargetFormat);
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = mDepthStencilFormat;
    THROW_IF_FAILED(mDX12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState->mDX12PipleLineState)));

    return pipelineState;

}

FRHITexture2D* FD3D12RHI::CreateTexture2D(const std::wstring& filePathName)
{
    FD3D12Texture2D* texture2D = new FD3D12Texture2D;

    FlushCommandQueue();

    //reset command list
    THROW_IF_FAILED(mDX12CommandList->Reset(mDX12CommandAllocator[mFrameIndex].Get(), NULL));

    THROW_IF_FAILED(DirectX::CreateDDSTextureFromFile12(mDX12Device.Get(),
        mDX12CommandList.Get(), filePathName.c_str(),
        texture2D->mTexture, texture2D->mTextureUploadHeap));


    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorObj(mCBVSRVUAVHeap->GetCPUDescriptorHandleForHeapStart(), msCBVSRVUAVCount, mCBVSRVVUAVDescriptorSize);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = texture2D->mTexture->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = texture2D->mTexture->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    mDX12Device->CreateShaderResourceView(texture2D->mTexture.Get(), &srvDesc, descriptorObj);

    texture2D->PosInHeapCBVSRVUAV = msCBVSRVUAVCount;

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

    renderTarget->Init();

    if (formatTarget != PF_UNKNOWN)
    {
        for (uint32 i = 0; i < numTarget; i++)
        {
            FD3D12Texture2D* renderTexture = new FD3D12Texture2D;

            renderTarget->RenderTargets[i] = renderTexture;

            DXGI_FORMAT pixelFormat = translatePixelFormat(formatTarget);
            //create render view
            CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(mDX12DescriptorHeapRenderTarget->GetCPUDescriptorHandleForHeapStart(), msRTVCount, mRTVDescriptorSize);
            D3D12_RESOURCE_DESC renderTargetDesc;
            renderTargetDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            renderTargetDesc.Alignment = 0;
            renderTargetDesc.Width = renderTarget->Width;
            renderTargetDesc.Height = renderTarget->Height;
            renderTargetDesc.DepthOrArraySize = 1;
            renderTargetDesc.MipLevels = 1;
            renderTargetDesc.Format = pixelFormat;
            renderTargetDesc.SampleDesc.Count = 1;
            renderTargetDesc.SampleDesc.Quality = 0;
            renderTargetDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            renderTargetDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

            renderTexture->PosInHeapRTVDSV = msRTVCount;

            msRTVCount++;

            D3D12_CLEAR_VALUE clearValue;
            clearValue.Color[0] = 0.5f;
            clearValue.Color[1] = 0.5f;
            clearValue.Color[2] = 0.5f;
            clearValue.Color[3] = 1.0f;
            clearValue.Format = pixelFormat;
            CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

            THROW_IF_FAILED(mDX12Device->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &renderTargetDesc,
                D3D12_RESOURCE_STATE_COMMON,
                &clearValue,
                IID_PPV_ARGS(&renderTexture->mDX12Resource)));

            mDX12Device->CreateRenderTargetView(renderTexture->GetDX12Resource().Get(), nullptr, descriptorHandle);

            CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorObj(mCBVSRVUAVHeap->GetCPUDescriptorHandleForHeapStart(), msCBVSRVUAVCount, mCBVSRVVUAVDescriptorSize);

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = renderTexture->GetDX12Resource()->GetDesc().Format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = renderTexture->GetDX12Resource()->GetDesc().MipLevels;
            srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

            renderTexture->PosInHeapCBVSRVUAV = msCBVSRVUAVCount;
            mDX12Device->CreateShaderResourceView(renderTexture->GetDX12Resource().Get(), &srvDesc, descriptorObj);
            msCBVSRVUAVCount++;

        }
    }

    if (formatDepthStencil != PF_UNKNOWN)
    {
        FD3D12Texture2D* depthStencil = new FD3D12Texture2D;
        renderTarget->DepthStencilTarget = depthStencil;

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
            IID_PPV_ARGS(&depthStencil->mDX12Resource)));

        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandleDS = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDX12DescriptorHeapDepthStencil->GetCPUDescriptorHandleForHeapStart(), msDSVCount, mDSVDescriptorSize);
        //create descriptor to mip level 0 of entire resource using the format of the resource.
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.Texture2D.MipSlice = 0;
        mDX12Device->CreateDepthStencilView(depthStencil->GetDX12Resource().Get(), &dsvDesc, descriptorHandleDS);

        depthStencil->PosInHeapRTVDSV = msDSVCount;

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
        mDX12Device->CreateShaderResourceView(depthStencil->GetDX12Resource().Get(), &srvDesc, descriptorObj);

        depthStencil->PosInHeapCBVSRVUAV = msCBVSRVUAVCount;

        msCBVSRVUAVCount++;
    }

    return renderTarget;
}

FRHIRenderWindow* FD3D12RHI::CreateRenderWindow(uint32 width, uint32 hight)
{
    FD3D12RenderWindow* renderTarget = new FD3D12RenderWindow(width, hight, DXGI_FORMAT_R8G8B8A8_UNORM, FRAME_BUFFER_NUM);

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
    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(mDX12DescriptorHeapRenderTarget->GetCPUDescriptorHandleForHeapStart(), msRTVCount, mRTVDescriptorSize);

    for (uint32 i = 0; i < renderTarget->NumTarget; i++)
    {
        FD3D12Texture2D* renderTexture = new FD3D12Texture2D;

        renderTarget->RenderTargets[i] = renderTexture;

        renderTarget->mDXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&renderTexture->mDX12Resource));
        mDX12Device->CreateRenderTargetView(renderTexture->GetDX12Resource().Get(), nullptr, descriptorHandle);
        descriptorHandle.Offset(1, mRTVDescriptorSize);

        renderTexture->PosInHeapRTVDSV = msRTVCount;

        msRTVCount++;
    }

    FD3D12Texture2D* depthStencil = new FD3D12Texture2D;

    renderTarget->DepthStencilTarget = depthStencil;

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
    clearValue.Format = mDepthStencilFormat;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;
    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

    THROW_IF_FAILED(mDX12Device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &clearValue,
        IID_PPV_ARGS(&depthStencil->mDX12Resource)));

    D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandleDS = CD3DX12_CPU_DESCRIPTOR_HANDLE(
        mDX12DescriptorHeapDepthStencil->GetCPUDescriptorHandleForHeapStart(), msDSVCount, mDSVDescriptorSize);
    //create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.Texture2D.MipSlice = 0;
    mDX12Device->CreateDepthStencilView(depthStencil->GetDX12Resource().Get(), &dsvDesc, descriptorHandleDS);

    depthStencil->PosInHeapRTVDSV = msDSVCount;

    msDSVCount++;

    ID3D12CommandList* cmdsLists[] = { mDX12CommandList.Get() };
    mDX12CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

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

void FD3D12RHI::TransitionResource(const FRHITransitionInfo& info)
{
    FD3D12Resource* resource = dynamic_cast<FD3D12Resource*>(info.Resource);

    if (resource != nullptr)
    {
        const D3D12_RESOURCE_STATES before = translateResourceTransitionAccess(info.AccessBefore);
        const D3D12_RESOURCE_STATES after = translateResourceTransitionAccess(info.AccessAfter);

        CD3DX12_RESOURCE_BARRIER resourceBarries = CD3DX12_RESOURCE_BARRIER::Transition(resource->GetDX12Resource().Get(), before, after);
        mDX12CommandList->ResourceBarrier(1, &resourceBarries);
    }
}

void FD3D12RHI::Present(FRHIRenderWindow* window)
{
    window->Present();

    mFrameIndex = window->GetRenderTargetIndex();
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
    rtDescriptorHeapDesc.NumDescriptors = 256;
    rtDescriptorHeapDesc.NodeMask = 0;
    THROW_IF_FAILED(mDX12Device->CreateDescriptorHeap(&rtDescriptorHeapDesc, IID_PPV_ARGS(&mDX12DescriptorHeapRenderTarget)));

    D3D12_DESCRIPTOR_HEAP_DESC dsDescriptorHeapDesc;
    dsDescriptorHeapDesc.NumDescriptors = 256;
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

D3D12_RESOURCE_STATES FD3D12RHI::translateResourceTransitionAccess(EResourceTransitionAccess access)
{
    return (D3D12_RESOURCE_STATES)access;

}
DXGI_FORMAT FD3D12RHI::translatePixelFormat(EPixelFormat format)
{
    return (DXGI_FORMAT)format;
}

D3D12_COMPARISON_FUNC FD3D12RHI::translateCompareFunction(ECompareFunction CompareFunction)
{
    switch (CompareFunction)
    {
    case CF_Less: return D3D12_COMPARISON_FUNC_LESS;
    case CF_LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case CF_Greater: return D3D12_COMPARISON_FUNC_GREATER;
    case CF_GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    case CF_Equal: return D3D12_COMPARISON_FUNC_EQUAL;
    case CF_NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case CF_Never: return D3D12_COMPARISON_FUNC_NEVER;
    default: return D3D12_COMPARISON_FUNC_ALWAYS;
    };
}

D3D12_BLEND_OP FD3D12RHI::translateBlendOp(EBlendOperation BlendOp)
{
    switch (BlendOp)
    {
    case BO_Subtract: return D3D12_BLEND_OP_SUBTRACT;
    case BO_Min: return D3D12_BLEND_OP_MIN;
    case BO_Max: return D3D12_BLEND_OP_MAX;
    case BO_ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
    default: return D3D12_BLEND_OP_ADD;
    };
}

D3D12_BLEND FD3D12RHI::translateBlendFactor(EBlendFactor BlendFactor)
{
    switch (BlendFactor)
    {
    case BF_One: return D3D12_BLEND_ONE;
    case BF_SourceColor: return D3D12_BLEND_SRC_COLOR;
    case BF_InverseSourceColor: return D3D12_BLEND_INV_SRC_COLOR;
    case BF_SourceAlpha: return D3D12_BLEND_SRC_ALPHA;
    case BF_InverseSourceAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
    case BF_DestAlpha: return D3D12_BLEND_DEST_ALPHA;
    case BF_InverseDestAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
    case BF_DestColor: return D3D12_BLEND_DEST_COLOR;
    case BF_InverseDestColor: return D3D12_BLEND_INV_DEST_COLOR;
    case BF_ConstantBlendFactor: return D3D12_BLEND_BLEND_FACTOR;
    case BF_InverseConstantBlendFactor: return D3D12_BLEND_INV_BLEND_FACTOR;
    default: return D3D12_BLEND_ZERO;
    };
}

D3D12_CULL_MODE FD3D12RHI::translateCullMode(ERasterizerCullMode CullMode)
{
    switch (CullMode)
    {
    case CM_CW: return D3D12_CULL_MODE_BACK;
    case CM_CCW: return D3D12_CULL_MODE_FRONT;
    default: return D3D12_CULL_MODE_NONE;
    };
}
