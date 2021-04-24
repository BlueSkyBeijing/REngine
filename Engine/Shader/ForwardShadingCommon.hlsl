#include "ShadingCommon.hlsl"

struct VertexIn
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
    float4 Pos : SV_POSITION;
    float4 ShadowPosH : POSITION0;
    float4 PosW : POSITION1;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

cbuffer ObjectConstantBuffer : register(b0)
{
    float4x4 World;
};

cbuffer MainPassConstantBuffer : register(b1)
{
    float4x4 View;
    float4x4 Proj;
    float4x4 ViewProj;
    float4x4 ShadowTransform;
    float3 CameraPos;
    float _placeholder0;
    float3 CameraDir;
    float _placeholder1;
    float3 DirectionalLightDir;
    float _placeholder2;
    float3 DirectionalLightColor;
    float _placeholder3;
    float2 InvShadowMapSize;
};

