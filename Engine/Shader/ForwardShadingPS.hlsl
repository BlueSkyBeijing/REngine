#include "ForwardShadingCommon.hlsl"
#include "ShadowFiltering.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);
TextureCube EnvironmentMap : register(t1);
SamplerState EnvironmentMapSampe : register(s2);


#define REFLECTION_CAPTURE_ROUGHEST_MIP 1
#define REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE 1.2

/** 
 * Compute absolute mip for a reflection capture cubemap given a roughness.
 */
float ComputeReflectionCaptureMipFromRoughness(float Roughness, float CubemapMaxMip)
{
	// Heuristic that maps roughness to mip level
	// This is done in a way such that a certain mip level will always have the same roughness, regardless of how many mips are in the texture
	// Using more mips in the cubemap just allows sharper reflections to be supported
    float LevelFrom1x1 = REFLECTION_CAPTURE_ROUGHEST_MIP - REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE * log2(Roughness);
    return CubemapMaxMip - 1 - LevelFrom1x1;
}

float4 PSMain(VertexShaderOutput pixelIn) : SV_Target
{
    float4 outColor;
    
    const float4 baseColor = DiffuseMap.Sample(DiffuseSamplerState, pixelIn.UV);
    //view dir is different from camare dir,it's diffent in every pixel
    const float3 viewDir = normalize(CameraPos - pixelIn.PosW.xyz);
    const float2 shadowAndThickness = DirectionalLightShadow(pixelIn.ShadowPosH);
    const float shadow = shadowAndThickness.x;
    const float thickness = shadowAndThickness.y; 
    
    // Compute fractional mip from roughness
    float roughness = 0.5f;
    float3 specularColor = DirectionalLightColor;
    float3 r = reflect(-CameraPos, pixelIn.Normal);
    float AbsoluteSpecularMip = ComputeReflectionCaptureMipFromRoughness(roughness, 10.0f);
    float4 reflectionColor = EnvironmentMap.SampleLevel(EnvironmentMapSampe, r, AbsoluteSpecularMip);
    float NoV = max(dot(DirectionalLightColor,  pixelIn.Normal), 0.0);

    float3 IndirectSpecularPart = EnvBRDFApprox(specularColor, roughness, NoV);
    float3 evn = IndirectSpecularPart * RGBMDecode(reflectionColor, 16.0f);
    
    float3 lighting = DirectionalLighting(pixelIn.Normal, DirectionalLightDir, DirectionalLightColor, DirectionalLightIntensity, viewDir, baseColor.rgb, shadow, thickness);

    for (int i = 0; i < PointLightNum; ++i)
    {
        lighting += PointLighting(pixelIn.Normal, PointLightPositionAndInvRadius[i].xyz, PointLightColorAndFalloffExponent[i].xyz,  PointLightIntensity[(uint)i/(MAX_POINT_LIGHT_NUM/4)][(uint)i%(MAX_POINT_LIGHT_NUM/4)], PointLightPositionAndInvRadius[i].w, pixelIn.PosW.xyz, viewDir, baseColor.xyz, shadow);
    }

    outColor.rgb = lighting;
    outColor.rgb += evn;
    outColor.a = Opacity;
    
    return outColor;
}

