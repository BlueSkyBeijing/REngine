#include "ForwardShadingCommon.hlsl"
#include "ShadowDepthCommon.hlsl"

cbuffer ShadowPassConstantBuffer : register(b1)
{
    float4x4 ShadowView;
    float4x4 ShadowProj;
    float4x4 ShadowViewProj;
};


ShadowVertexOut VSMain(VertexShaderInput vertexIn)
{
    ShadowVertexOut vertexOut;
    
#ifdef GPU_SKIN
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vertexIn.BoneWeights.x;
    weights[1] = vertexIn.BoneWeights.y;
    weights[2] = vertexIn.BoneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    float3 pos = float3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; ++i)
    {
        pos += weights[i] * mul(float4(vertexIn.Pos, 1.0f), BoneTransforms[vertexIn.BoneIndices[i]]).xyz;
    }

    vertexIn.Pos = pos;
#endif
    float4x4 worldViewProj = mul(World, ShadowViewProj);
    vertexOut.Pos = mul(float4(vertexIn.Pos, 1.0f), worldViewProj);
    vertexOut.UV = vertexIn.UV;

    return vertexOut;
}