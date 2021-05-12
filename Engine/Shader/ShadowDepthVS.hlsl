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
    
#ifdef GPU_SKIN
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vin.BoneWeights.x;
    weights[1] = vin.BoneWeights.y;
    weights[2] = vin.BoneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    float3 pos = float3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; ++i)
    {
        pos += weights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;
    }

    vin.Pos = pos;
#endif
    float4x4 worldViewProj = mul(World, ShadowViewProj);
    vertexOut.Pos = mul(float4(vertexIn.Pos, 1.0f), worldViewProj);
    
    return vertexOut;
}