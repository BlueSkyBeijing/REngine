#include "ForwardShadingCommon.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSamplerState : register(s0);

float4 PSMain(VertexOut PIn) : SV_Target
{
    float4 outColor;
    
    float ambient = 0.1f;
    float4 diffuse = DiffuseMap.Sample(DiffuseSamplerState, PIn.TexCoord);
    const float specPower = 2.0f;
    const float3 viewDir = gCameraPos - PIn.Pos;
    float3 lighting = BlinnPhong(PIn.Normal, gDirectionalLightDir, gDirectionalLightColor, gDirectionalLightColor, specPower, viewDir);

    outColor.rgb = diffuse.rgb * (ambient + lighting);
    outColor.a = 1.0f;
    
    return outColor;
}

