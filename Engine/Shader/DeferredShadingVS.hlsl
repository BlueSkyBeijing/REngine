#include "DeferredShadingCommon.hlsl"

cbuffer CBObject : register(b0)
{
    float4x4 WorldViewProj;
};

VertexOut VSMain(VertexIn vertexIn)
{
    VertexOut VOut;
	
    vertexOut.Pos = mul(float4(vertexIn.Pos, 1.0f), WorldViewProj);
	
    vertexOut.Color = vertexIn.Color;
 
    vertexOut.TexCoord = vertexIn.TexCoord;

    return vertexOut;
}