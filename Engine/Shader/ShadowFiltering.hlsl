Texture2D ShadowMap : register(t1);
SamplerComparisonState SamperShadow : register(s1);

#define PCF_COUNT 9

float DirectionalLightShadow(float4 shadowPosH)
{
    //complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;

    //depth in NDC space.
    float depth = shadowPosH.z;

    uint width, height, numMips;
    ShadowMap.GetDimensions(0, width, height, numMips);

    //texel size.
    float dx = 1.0f / (float)width;

    float percentLit = 0.0f;
    const float2 offsets[PCF_COUNT] =
    {
        float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    };

    [unroll]
    for (int i = 0; i < PCF_COUNT; ++i)
    {
        percentLit += ShadowMap.SampleCmpLevelZero(SamperShadow,
            shadowPosH.xy + offsets[i], depth).r;
    }
    
    return percentLit / PCF_COUNT;
}

