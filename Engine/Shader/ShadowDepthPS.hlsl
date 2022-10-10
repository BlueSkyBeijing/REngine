#include "ForwardShadingCommon.hlsl"
#include "ShadowDepthCommon.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);


float4 PSMain(ShadowVertexOut pixelIn) : SV_Target
{
    float4 outColor;
    const float4 baseColor = DiffuseMap.Sample(DiffuseSamplerState, pixelIn.UV);

    outColor.rgb = 0.0f;
    outColor.a = baseColor.a * Opacity;
    
    #if MATERIALBLENDING_MASKED
    const float maskClipValue = 0.33f;
    clip(outColor.a - maskClipValue);
    #endif

    return outColor;
}
