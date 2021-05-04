
struct VertexShaderInput
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
};

struct VertexShaderOutput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

VertexShaderOutput TonemapVS(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    output.Pos = float4(input.Pos, 1.0f);
    output.UV = input.UV;
    
    return output;

}

Texture2D SceneColorTexture : register(t0);
SamplerState SceneColorTextureSampler : register(s0);

Texture2D BloomUpTexture : register(t1);
SamplerState BloomUpTextureSampler : register(s1);

float4 TonemapPS(VertexShaderOutput input) : SV_TARGET
{
    float4 sceneColor = SceneColorTexture.Sample(SceneColorTextureSampler, input.UV.xy).rgba;

    float4 color = BloomUpTexture.Sample(BloomUpTextureSampler, input.UV.xy);

    color += lerp(sceneColor, color, 0.2f);
    
    color = color / (1.0f + color);
    
    return color;
}
