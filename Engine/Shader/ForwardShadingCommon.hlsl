#include "ShadingCommon.hlsl"

#define MAX_POINT_LIGHT_NUM 4

struct VertexShaderInput
{
    float3 Pos : POSITION;
    float4 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float3 Tangent : TANGENT;
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
    float4 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
    float3 TangentToWorld0 : TEXCOORD1_centroid;
    float4 TangentToWorld2 : TEXCOORD2_centroid;

};

cbuffer ObjectConstantBuffer : register(b0)
{
    float4x4 World;
#ifdef GPU_SKIN
    float4x4 BoneTransforms[75];
#endif
    float4 EmissiveColorScale;
    float4 SubsurfaceColorScale;
    
    float MetallicScale;
    float SpecularScale;
    float RoughnessScale;
    float OpacityScale;   
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
    float DirectionalLightShadowBias;
   
    int PointLightNum;
    float4 PointLightPositionAndInvRadius[MAX_POINT_LIGHT_NUM];
    float4 PointLightColorAndFalloffExponent[MAX_POINT_LIGHT_NUM];
    float4 PointLightIntensity[MAX_POINT_LIGHT_NUM/4];
};

