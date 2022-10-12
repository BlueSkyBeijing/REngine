#include "ForwardShadingCommon.hlsl"
#include "ShadowFiltering.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);


float4 PSMain(VertexShaderOutput pixelIn, bool IsFrontFace: SV_IsFrontFace) : SV_Target
{
    float twoSideSign = IsFrontFace ? -1 : 1;
    float4 outColor;
    
    //view dir is different from camare dir,it's diffent in every pixel
    const float3 viewDir = normalize(CameraPos - pixelIn.PosW.xyz); 
    const float4 baseColor = DiffuseMap.Sample(DiffuseSamplerState, pixelIn.UV);
    const float2 shadowAndThickness = DirectionalLightShadow(pixelIn.ShadowPosH);
    const float opacity = baseColor.a * Opacity;
    
    MaterialContext matContext;
    InitMaterialContext(matContext, baseColor.rgb, Metallic, Specular, Roughness, opacity, EmissiveColor.rgb, SubsurfaceColor.rgb);
    CalculateDiffuseAndSpecularColor(matContext.Specular, matContext.Metallic, matContext.DiffuseColor, matContext.SpecularColor);

    LightingContext litContextDirectional;
    InitLightingContext(litContextDirectional, DirectionalLightDir, DirectionalLightIntensity, DirectionalLightColor, twoSideSign * pixelIn.Normal, viewDir, CameraPos, 0, 0, 0, shadowAndThickness.x, shadowAndThickness.y);

    float3 lighting = DirectionalLighting(litContextDirectional, matContext);
    for (int i = 0; i < PointLightNum; ++i)
    {
        uint indexArray = (uint)i/(MAX_POINT_LIGHT_NUM/4);
        uint indexComp = (uint)i%(MAX_POINT_LIGHT_NUM/4);
        float pointLightIntensity = PointLightIntensity[indexArray][indexComp];
        LightingContext litContextPoint;
        InitLightingContext(litContextPoint, DirectionalLightDir, pointLightIntensity, PointLightColorAndFalloffExponent[i].xyz, pixelIn.Normal, viewDir, CameraPos, PointLightPositionAndInvRadius[i].xyz, PointLightPositionAndInvRadius[i].w, pixelIn.PosW.xyz, shadowAndThickness.x, shadowAndThickness.y);

        lighting += PointLighting(litContextPoint, matContext);
    }

    float3 envReflectionColor = GetImageBasedReflectionLighting(litContextDirectional, matContext);
    lighting += envReflectionColor;
    
    float3 envDiffuseColor = GetImageBasedDiffuseLighting(litContextDirectional, matContext);
    lighting += envDiffuseColor;
    
    outColor.rgb = lighting;
    outColor.a = opacity;
    
    #if MATERIALBLENDING_MASKED
    const float maskClipValue = 0.33f;
    clip(outColor.a - maskClipValue);
    #endif

    return outColor;
}
