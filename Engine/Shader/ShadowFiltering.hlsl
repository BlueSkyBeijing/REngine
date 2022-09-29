Texture2D ShadowMap : register(t1);
SamplerState SamperShadow : register(s1);

#define PCF_COUNT 9

float2 DirectionalLightShadow(float4 shadowPosH)
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

    float2 shadowAndThickness = float2(1, 1);
    [unroll]
    for (int i = 0; i < PCF_COUNT; ++i)
    {
        float curShadow = ShadowMap.Sample(SamperShadow,
            (shadowPosH.xy + offsets[i])).r;
        curShadow = saturate(sign(curShadow - depth));
        shadowAndThickness.x += curShadow;
        shadowAndThickness.y += 1-exp(-abs(curShadow - depth));
    }
    
    return shadowAndThickness / PCF_COUNT;
}

