// ACES
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

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
SamplerState PointClampTextureSampler : register(s3);
SamplerState LinearClampTextureSampler : register(s2);

Texture2D BloomTexture : register(t1);

float4 TonemapPS(VertexShaderOutput input) : SV_TARGET
{
    float4 sceneColor = SceneColorTexture.Sample(PointClampTextureSampler, input.UV.xy).rgba;

    float4 color = sceneColor;

    color += BloomTexture.Sample(LinearClampTextureSampler, input.UV.xy);
    
    color.rgb = ACESFilm(color.rgb);
    
    return color;
}
