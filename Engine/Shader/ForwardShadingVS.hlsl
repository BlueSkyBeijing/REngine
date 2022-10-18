#include "ForwardShadingCommon.hlsl"

VertexShaderOutput VSMain(VertexShaderInput vertexIn)
{
    VertexShaderOutput vertexOut;

#ifdef GPU_SKIN
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vertexIn.BoneWeights.x;
    weights[1] = vertexIn.BoneWeights.y;
    weights[2] = vertexIn.BoneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    float3 pos = float3(0.0f, 0.0f, 0.0f);
    float3 normal = float3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; ++i)
    {
        pos += weights[i] * mul(float4(vertexIn.Pos, 1.0f), BoneTransforms[vertexIn.BoneIndices[i]]).xyz;
        normal += weights[i] * mul(vertexIn.Normal, (float3x3)BoneTransforms[vertexIn.BoneIndices[i]]);
    }

    vertexIn.Pos = pos;
    vertexIn.Normal = normal;
#endif
	
    float4x4 worldViewProj = mul(World, ViewProj);
    vertexOut.Pos = mul(float4(vertexIn.Pos, 1.0f), worldViewProj);
    vertexOut.PosW = mul(float4(vertexIn.Pos, 1.0f), World);
    vertexOut.ShadowPosH = mul(vertexOut.PosW, ShadowTransform);
    vertexOut.Normal = normalize(mul(vertexIn.Normal, (float3x3) World));
    vertexOut.Tangent = normalize(mul(vertexIn.Tangent, (float3x3) World));
    vertexOut.UV = vertexIn.UV;
    
    float3x3 TangentToLocal = CalcTangentToLocal(vertexIn.Tangent, vertexIn.Normal, 1);
    float3x3 TangentToWorld = mul(TangentToLocal, (float3x3) World);
    vertexOut.TangentToWorld0 = float3(TangentToWorld[0]);
    vertexOut.TangentToWorld2 = float3(TangentToWorld[2]);

    return vertexOut;
}