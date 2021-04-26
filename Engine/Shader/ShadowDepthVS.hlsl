#include "ForwardShadingCommon.hlsl"

cbuffer ShadowPassConstantBuffer : register(b1)
{
    float4x4 ShadowView;
    float4x4 ShadowProj;
    float4x4 ShadowViewProj;
};

struct ShadowVertexOut
{
    float4 Pos : SV_POSITION;
};

ShadowVertexOut VSMain(VertexShaderInput vertexIn)
{
    ShadowVertexOut vertexOut;
	
    float4x4 worldViewProj = mul(World, ShadowViewProj);
    vertexOut.Pos = mul(float4(vertexIn.Pos, 1.0f), worldViewProj);
    
    return vertexOut;
}