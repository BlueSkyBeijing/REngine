#include "ForwardShadingCommon.hlsl"
#include "ShadowFiltering.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);

Texture2D NormalMap : register(t4);
SamplerState NormalSamplerState : register(s4);

Texture2D MetallicSpecularRoughnessMap : register(t5);
SamplerState MetallicSpecularRoughnessSamplerState : register(s5);

Texture2D EmissiveColorMap : register(t6);
SamplerState EmissiveColorSamplerState : register(s6);

Texture2D SubsurfaceColorMap : register(t7);
SamplerState SubsurfaceColorSamplerState : register(s7);


float4 PSMain(VertexShaderOutput pixelIn, bool IsFrontFace: SV_IsFrontFace) : SV_Target
{
    float twoSideSign = IsFrontFace ? -1 : 1;
    float3x3 tangentToWorld = AssembleTangentToWorld(pixelIn.TangentToWorld0, pixelIn.TangentToWorld2);
    float4 noramlMap = normalize(NormalMap.Sample(NormalSamplerState, pixelIn.UV) * 2.0 - 1.0);
    float3 worldNormal = twoSideSign * TransformTangentNormalToWorld(tangentToWorld, noramlMap.xyz);

    float4 outColor;
    
    //view dir is different from camare dir,it's diffent in every pixel
    const float3 viewDir = normalize(CameraPos - pixelIn.PosW.xyz); 
    float4 baseColor = DiffuseMap.Sample(DiffuseSamplerState, pixelIn.UV);
    baseColor *= BaseColorScale;
    const float2 shadowAndThickness = DirectionalLightShadow(pixelIn.ShadowPosH);
    const float opacity = baseColor.a * OpacityScale;
    const float4 metallicSpecularRoughness = MetallicSpecularRoughnessMap.Sample(MetallicSpecularRoughnessSamplerState, pixelIn.UV);
    const float metallic = metallicSpecularRoughness.r * MetallicScale;
    const float specular = metallicSpecularRoughness.g * SpecularScale;
    const float roughness = metallicSpecularRoughness.b * RoughnessScale;
    float4 emissiveColor = EmissiveColorMap.Sample(EmissiveColorSamplerState, pixelIn.UV);
    emissiveColor *= EmissiveColorScale;
    float4 subsurfaceColor = SubsurfaceColorMap.Sample(SubsurfaceColorSamplerState, pixelIn.UV);
    subsurfaceColor *= SubsurfaceColorScale;

    MaterialContext matContext;
    InitMaterialContext(matContext, baseColor.rgb, metallic, specular, roughness, opacity, emissiveColor.rgb, subsurfaceColor.rgb);
    CalculateDiffuseAndSpecularColor(matContext.Specular, matContext.Metallic, matContext.DiffuseColor, matContext.SpecularColor);

    LightingContext litContextDirectional;
    InitLightingContext(litContextDirectional, DirectionalLightDir, DirectionalLightIntensity, DirectionalLightColor, worldNormal, viewDir, CameraPos, 0, 0, 0, shadowAndThickness.x, shadowAndThickness.y);

    float3 lighting = Lighting(litContextDirectional, matContext);
    for (int i = 0; i < PointLightNum; ++i)
    {
        uint indexArray = (uint)i/(MAX_POINT_LIGHT_NUM/4);
        uint indexComp = (uint)i%(MAX_POINT_LIGHT_NUM/4);
        float pointLightIntensity = PointLightIntensity[indexArray][indexComp];     
        float4 lightDirAndIntensity = CaculatePointLightDirAndIntensity(PointLightPositionAndInvRadius[i].xyz, pixelIn.PosW.xyz, pointLightIntensity, PointLightPositionAndInvRadius[i].w);

        LightingContext litContextPoint;
        InitLightingContext(litContextPoint, lightDirAndIntensity.xyz, lightDirAndIntensity.w, PointLightColorAndFalloffExponent[i].xyz, worldNormal, viewDir, CameraPos, PointLightPositionAndInvRadius[i].xyz, PointLightPositionAndInvRadius[i].w, pixelIn.PosW.xyz, 1, 1);

        lighting += Lighting(litContextPoint, matContext);
    }

#if SHADING_MODEL != SHADING_MODEL_UNLIT
    float3 envReflectionColor = GetImageBasedReflectionLighting(litContextDirectional, matContext);
    lighting += envReflectionColor;
    
    float3 envDiffuseColor = GetImageBasedDiffuseLighting(litContextDirectional, matContext);
    lighting += envDiffuseColor;
#endif

    outColor.rgb = lighting;
    outColor.a = opacity;
    
    #if MATERIALBLENDING_MASKED
    const float maskClipValue = 0.33f;
    clip(outColor.a - maskClipValue);
    #endif

    return outColor;
}
