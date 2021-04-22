#include "ForwardShadingCommon.hlsl"

float4 PSMain(VertexOut pixelIn) : SV_Target
{
    float4 outColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
        
    return outColor;
}

