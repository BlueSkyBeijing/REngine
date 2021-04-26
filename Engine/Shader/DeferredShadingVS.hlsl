#include "DeferredShadingCommon.hlsl"

cbuffer CBObject : register(b0)
{
    float4x4 WorldViewProj;
};

VertexShaderOutput VSMain(VertexShaderInput vertexIn)
{
    VertexShaderOutput VOut;
	
    vertexOut.Pos = mul(float4(vertexIn.Pos, 1.0f), WorldViewProj);
	
    vertexOut.Color = vertexIn.Color;
 
    vertexOut.UV = vertexIn.UV;

    return vertexOut;
}