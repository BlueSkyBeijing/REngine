#include "ForwardShadingCommon.hlsl"

VertexShaderOutput VSMain(VertexShaderInput vertexIn)
{
    VertexShaderOutput vertexOut;

#ifdef GPU_SKIN
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vin.BoneWeights.x;
    weights[1] = vin.BoneWeights.y;
    weights[2] = vin.BoneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    float3 pos = float3(0.0f, 0.0f, 0.0f);
    float3 normal = float3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; ++i)
    {
        pos += weights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;
        normal += weights[i] * mul(vin.NormalL, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
    }

    vin.Pos = pos;
    vin.Normal = normal;
#endif
	
    float4x4 worldViewProj = mul(World, ViewProj);
    vertexOut.Pos = mul(float4(vertexIn.Pos, 1.0f), worldViewProj);
    vertexOut.PosW = mul(float4(vertexIn.Pos, 1.0f), World);
    vertexOut.ShadowPosH = mul(vertexOut.PosW, ShadowTransform);
    vertexOut.Normal = normalize(mul(vertexIn.Normal, (float3x3) World));
    vertexOut.UV = vertexIn.UV;

    return vertexOut;
}