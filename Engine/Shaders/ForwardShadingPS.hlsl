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
    const float shadow = DirectionalLightShadow(pixelIn.ShadowPosH).x;
    float3 lighting = BlinnPhong(pixelIn.Normal.xyz, DirectionalLightDir, DirectionalLightColor, DirectionalLightIntensity, viewDir, diffuseColor.rgb, shadow);

    for (int i = 0; i < PointLightNum; ++i)
    {
        lighting += PointLighting(pixelIn.Normal.xyz, PointLightPositionAndInvRadius[i].xyz, PointLightColorAndFalloffExponent[i].xyz, PointLightPositionAndInvRadius[i].w, pixelIn.PosW.xyz, diffuseColor.xyz);
    }

    outColor.rgb = lighting;
    outColor.a = OpacityScale;
    
    return outColor;
}

