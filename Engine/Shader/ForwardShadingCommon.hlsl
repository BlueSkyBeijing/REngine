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
    float4 PosW : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

cbuffer cbObject : register(b0)
{
    float4x4 World;
};

cbuffer cbPass : register(b1)
{
    float4x4 View;
    float4x4 Proj;
    float4x4 ViewProj;
    float3 CameraPos;
    float _Placeholder0;
    float3 CameraDir;
    float _Placeholder1;
    float3 DirectionalLightDir;
    float _Placeholder2;
    float3 DirectionalLightColor;
    float _Placeholder3;
};

