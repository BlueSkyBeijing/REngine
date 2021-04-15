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
    float4x4 gWorld;
};

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
    float3 gCameraPos;
    float _Placeholder0;
    float3 gCameraDir;
    float _Placeholder1;
    float3 gDirectionalLightDir;
    float _Placeholder2;
    float3 gDirectionalLightColor;
    float _Placeholder3;
};

