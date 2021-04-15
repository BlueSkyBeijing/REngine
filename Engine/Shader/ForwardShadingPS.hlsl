#include "ForwardShadingCommon.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);

float4 PSMain(VertexOut PIn) : SV_Target
{
    float4 outColor;
    
    const float4 diffuseColor = DiffuseMap.Sample(DiffuseSamplerState, PIn.TexCoord);
    const float3 viewDir = gCameraPos - PIn.Pos.xyz;
    const float lightIntensity = 1.0f;
    float3 lighting = BlinnPhong(PIn.Normal, gDirectionalLightDir, gDirectionalLightColor, lightIntensity, viewDir, diffuseColor.rgb);

    outColor.rgb = lighting;
    outColor.a = 1.0f;
    
    return outColor;
}

