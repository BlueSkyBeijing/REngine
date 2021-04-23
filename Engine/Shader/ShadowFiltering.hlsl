Texture2D ShadowMap : register(t1);
SamplerComparisonState SamperShadow : register(s1);

float DirectionalLightShadow(float4 shadowPosH)
{
    shadowPosH.xyz /= shadowPosH.w;

    float depth = shadowPosH.z;

    float shadow = ShadowMap.SampleCmpLevelZero(SamperShadow,
        shadowPosH.xy, depth).r;
    
    return shadow;
}

