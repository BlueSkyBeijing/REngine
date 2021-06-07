#include "ShadingCommon.hlsl"

#define MAX_POINT_LIGHT_NUM 4

struct VertexShaderInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
#ifdef GPU_SKIN
    uint4 BoneIndices  : BONEINDICES;
    float4 BoneWeights : WEIGHTS;
#endif
};

struct VertexShaderOutput
{
    float4 Pos : SV_POSITION;
    float4 ShadowPosH : POSITION0;
    float4 PosW : POSITION1;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

cbuffer ObjectConstantBuffer : register(b0)
{
    float4x4 World;
#ifdef GPU_SKIN
    float4x4 BoneTransforms[75];
#endif
    float Opacity;
};

cbuffer SceneColorPassConstantBuffer : register(b1)
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
    float DirectionalLightIntensity;
    float2 InvShadowMapSize;
    float _placeholder4;
   
    int PointLightNum;
    float4 PointLightPositionAndInvRadius[MAX_POINT_LIGHT_NUM];
    float4 PointLightColorAndFalloffExponent[MAX_POINT_LIGHT_NUM];
};

