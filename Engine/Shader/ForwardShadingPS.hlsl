#include "ForwardShadingCommon.hlsl"
#include "ShadowFiltering.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);

float4 PSMain(VertexShaderOutput pixelIn) : SV_Target
{
    float4 outColor;
    
    const float4 diffuseTint = float4(1.0f, 1.0f, 1.0f, 1.0f);
    const float4 diffuseColor = DiffuseMap.Sample(DiffuseSamplerState, pixelIn.UV) * diffuseTint;
    //view dir is different from camare dir,it's diffent in every pixel
    const float3 viewDir = normalize(CameraPos - pixelIn.PosW.xyz);
    const float lightIntensity = 1.5f;
    const float shadow = DirectionalLightShadow(pixelIn.ShadowPosH);
    float3 lighting = BlinnPhong(pixelIn.Normal, DirectionalLightDir, DirectionalLightColor, lightIntensity, viewDir, diffuseColor.rgb, shadow) * 0.1;

    for (int i = 0; i < PointLightNum; ++i)
    {
        lighting += PointLighting(pixelIn.Normal, PointLightPositionAndInvRadius[i].xyz, PointLightColorAndFalloffExponent[i].xyz, PointLightPositionAndInvRadius[i].w, pixelIn.PosW.xyz, diffuseColor.xyz);
    }

    outColor.rgb = lighting;
    outColor.a = Opacity;
    
    return outColor;
}

