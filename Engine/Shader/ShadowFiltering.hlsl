Texture2D ShadowMap : register(t1);
SamplerComparisonState SamperShadow : register(s1);

#define PCF_COUNT 9

float DirectionalLightShadow(float4 shadowPosH)
{
    shadowPosH.xyz /= shadowPosH.w;

    float depth = shadowPosH.z;

    float shadow = 0.0f;
    const float2 offsets[PCF_COUNT] =
    {
        float2(-InvShadowMapSize.x, -InvShadowMapSize.y), float2(0.0f, -InvShadowMapSize.y), float2(InvShadowMapSize.x, -InvShadowMapSize.y),
        float2(-InvShadowMapSize.x, 0.0f), float2(0.0f, 0.0f), float2(InvShadowMapSize.x, 0.0f),
        float2(-InvShadowMapSize.x, InvShadowMapSize.y), float2(0.0f, InvShadowMapSize.y), float2(InvShadowMapSize.x, InvShadowMapSize.y)
    };

    [unroll]
    for (int i = 0; i < PCF_COUNT; ++i)
    {
        shadow += ShadowMap.SampleCmpLevelZero(SamperShadow,
            shadowPosH.xy + offsets[i], depth).r;
    }
    
    return shadow / PCF_COUNT;
}

