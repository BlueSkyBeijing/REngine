#include "DeferredShadingCommon.hlsl"

cbuffer CBObject : register(b0)
{
    float4x4 WorldViewProj;
};

VertexOut VSMain(VertexIn VIn)
{
    VertexOut VOut;
	
    VOut.Pos = mul(float4(VIn.Pos, 1.0f), WorldViewProj);
	
    VOut.Color = VIn.Color;
 
    VOut.TexCoord = VIn.TexCoord;

    return VOut;
}