#include "ForwardShadingCommon.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);

float4 PSMain(VertexOut PIn) : SV_Target
{
    float4 outColor;
    
    float ambient = 0.5f;
    float4 diffuse = DiffuseMap.Sample(DiffuseSamplerState, PIn.TexCoord);
    const float specPower = 2.0f;
    float3 lighting = BlinnPhong(PIn.Normal, gDirectionalLightDir, gDirectionalLightColor, gDirectionalLightColor, specPower, gCameraDir);

    outColor.rgb = diffuse.rgb * (ambient + lighting);
    outColor.a = 1.0f;
    
    return outColor;
}

