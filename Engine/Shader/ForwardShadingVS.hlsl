#include "ForwardShadingCommon.hlsl"

VertexOut VSMain(VertexIn VIn)
{
    VertexOut VOut;
	
    float4x4 WorldViewProj = mul(gWorld, gViewProj);
    VOut.Pos = mul(float4(VIn.Pos, 1.0f), WorldViewProj);
    VOut.Normal = normalize(mul(VIn.Normal, (float3x3) gWorld));
    VOut.TexCoord = VIn.TexCoord;

    return VOut;
}