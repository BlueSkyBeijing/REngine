#include "ForwardShadingCommon.hlsl"

VertexOut VSMain(VertexIn vertexIn)
{
    VertexOut vertexOut;
	
    float4x4 worldViewProj = mul(World, ViewProj);
    vertexOut.Pos = mul(float4(vertexIn.Pos, 1.0f), worldViewProj);
    vertexOut.PosW = mul(float4(vertexIn.Pos, 1.0f), World);
    vertexOut.ShadowPosH = mul(vertexOut.PosW, ShadowTransform);
    vertexOut.Normal = normalize(mul(vertexIn.Normal, (float3x3) World));
    vertexOut.TexCoord = vertexIn.TexCoord;

    return vertexOut;
}