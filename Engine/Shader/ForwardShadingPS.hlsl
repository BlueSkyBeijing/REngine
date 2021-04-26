#include "ForwardShadingCommon.hlsl"
#include "ShadowFiltering.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);

float4 PSMain(VertexShaderOutput pixelIn) : SV_Target
{
    float4 outColor;
    
    const float4 diffuseTint = float4(1.0f, 0.9f, 0.1f, 1.0f);
    const float4 diffuseColor = DiffuseMap.Sample(DiffuseSamplerState, pixelIn.UV) * diffuseTint;
    //view dir is different from camare dir,it's diffent in every pixel
    const float3 viewDir = normalize(CameraPos - pixelIn.PosW.xyz);
    const float lightIntensity = 0.8f;
    const float shadow = DirectionalLightShadow(pixelIn.ShadowPosH);
    float3 lighting = BlinnPhong(pixelIn.Normal, DirectionalLightDir, DirectionalLightColor, lightIntensity, viewDir, diffuseTint.xyz, shadow);

    outColor.rgb = lighting;
    outColor.a = 1.0f;
    
    return outColor;
}

