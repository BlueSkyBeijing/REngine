#include "ForwardShadingCommon.hlsl"
#include "Common.hlsl"

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
        normal.xyz += weights[i] * mul(vertexIn.Normal.xyz, (float3x3)BoneTransforms[vertexIn.BoneIndices[i]]);
    }

    vertexIn.Pos = pos;
    vertexIn.Normal.xyz = normal.xyz;
#endif

    float4x4 worldViewProj = mul(World, ViewProj);
    vertexOut.Pos = mul(float4(vertexIn.Pos, 1.0f), worldViewProj);
    vertexOut.PosW = mul(float4(vertexIn.Pos, 1.0f), World);
    vertexOut.ShadowPosH = mul(vertexOut.PosW, ShadowTransform);
    vertexOut.Normal.xyz = normalize(mul(vertexIn.Normal.xyz, (float3x3) World));
    vertexOut.Normal.w = vertexIn.Normal.w;
    vertexOut.Tangent = normalize(mul(vertexIn.Tangent, (float3x3) World));
    vertexOut.UV = vertexIn.UV;
    
    float tangentSign;
    float3x3 tangentToLocal = CalcTangentToLocal(vertexIn.Tangent, vertexIn.Normal, tangentSign);
    float3x3 tangentToWorld = CalcTangentToWorld((float3x3) World, tangentToLocal);
    vertexOut.TangentToWorld0 = float3(tangentToWorld[0]);
    vertexOut.TangentToWorld2 = float4(tangentToWorld[2], tangentSign);

    return vertexOut;
}