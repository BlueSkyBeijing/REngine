#include "ForwardShadingCommon.hlsl"
#include "ShadowFiltering.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);

float4 PSMain(VertexShaderOutput pixelIn) : SV_Target
{
    float4 outColor;
    
    const float4 baseColor = DiffuseMap.Sample(DiffuseSamplerState, pixelIn.UV);
    //view dir is different from camare dir,it's diffent in every pixel
    const float3 viewDir = normalize(CameraPos - pixelIn.PosW.xyz);
    const float shadow = DirectionalLightShadow(pixelIn.ShadowPosH);
    
    float3 lighting = DirectionalLighting(pixelIn.Normal, DirectionalLightDir, DirectionalLightColor, DirectionalLightIntensity, viewDir, baseColor.rgb, shadow);

    for (int i = 0; i < PointLightNum; ++i)
    {
        lighting += PointLighting(pixelIn.Normal, PointLightPositionAndInvRadius[i].xyz, PointLightColorAndFalloffExponent[i].xyz,  PointLightIntensity[(uint)i/(MAX_POINT_LIGHT_NUM/4)][(uint)i%(MAX_POINT_LIGHT_NUM/4)], PointLightPositionAndInvRadius[i].w, pixelIn.PosW.xyz, viewDir, baseColor.xyz, shadow);
    }

    outColor.rgb = lighting;
    outColor.a = Opacity;
    
    return outColor;
}

