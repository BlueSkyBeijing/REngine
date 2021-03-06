#include "DeferredShadingCommon.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);

float4 PSMain(VertexShaderOutput PIn) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(DiffuseSamplerState, PIn.UV);

    return diffuse;
}

