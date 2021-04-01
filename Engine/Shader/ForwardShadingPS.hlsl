#include "ForwardShadingCommon.hlsl"
#include "Tonemap.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);

TextureCube EnvironmentMap : register(t1);

float4 PSMain(VertexOut PIn) : SV_Target
{
    float4 outColor;
    float4 diffuse = DiffuseMap.Sample(DiffuseSamplerState, PIn.TexCoord);

    outColor = diffuse;
    return outColor;
}

