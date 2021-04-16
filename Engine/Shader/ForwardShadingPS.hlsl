#include "ForwardShadingCommon.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);

float4 PSMain(VertexOut pixelIn) : SV_Target
{
    float4 outColor;
    
    const float4 diffuseColor = DiffuseMap.Sample(DiffuseSamplerState, pixelIn.TexCoord);
    const float3 viewDir = normalize(CameraPos - pixelIn.PosW.xyz);
    const float lightIntensity = 0.8f;
    float3 lighting = BlinnPhong(pixelIn.Normal, DirectionalLightDir, DirectionalLightColor, lightIntensity, viewDir, diffuseColor.rgb);

    outColor.rgb = lighting;
    outColor.a = 1.0f;
    
    return outColor;
}

