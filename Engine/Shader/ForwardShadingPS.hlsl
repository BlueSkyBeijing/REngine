#include "ForwardShadingCommon.hlsl"
#include "ShadowFiltering.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);


float4 PSMain(VertexShaderOutput pixelIn) : SV_Target
{
    float4 outColor;
    
    //view dir is different from camare dir,it's diffent in every pixel
    const float3 viewDir = normalize(CameraPos - pixelIn.PosW.xyz); 
    const float4 baseColor = DiffuseMap.Sample(DiffuseSamplerState, pixelIn.UV);
    
    MaterialContext matContext;
    InitMaterialContext(matContext, baseColor.rgb, Metallic, Specular, Roughness, Opacity);

    LightingContext litContextDirectional;
    InitLightingContext(litContextDirectional, DirectionalLightDir, DirectionalLightIntensity, DirectionalLightColor, pixelIn.Normal, viewDir, CameraPos);

    const float2 shadowAndThickness = DirectionalLightShadow(pixelIn.ShadowPosH);
    const float shadow = shadowAndThickness.x;
    const float thickness = shadowAndThickness.y; 

    float3 lighting = DirectionalLighting(litContextDirectional, matContext, shadow, thickness);
    for (int i = 0; i < PointLightNum; ++i)
    {
        float pointLightIntensity = PointLightIntensity[(uint)i/(MAX_POINT_LIGHT_NUM/4)][(uint)i%(MAX_POINT_LIGHT_NUM/4)];
        LightingContext litContextPoint;
        InitLightingContext(litContextPoint, DirectionalLightDir, pointLightIntensity, PointLightColorAndFalloffExponent[i].xyz, pixelIn.Normal, viewDir, CameraPos, PointLightPositionAndInvRadius[i].xyz, PointLightPositionAndInvRadius[i].w, pixelIn.PosW.xyz);

        lighting += PointLighting(litContextPoint, matContext, shadow, thickness);
    }

    float3 ReflectionColor = GetImageBasedReflectionLighting(litContextDirectional, matContext, shadow, thickness);
    lighting += ReflectionColor;
    outColor.rgb = lighting;
    outColor.a = Opacity;
    
    return outColor;
}
