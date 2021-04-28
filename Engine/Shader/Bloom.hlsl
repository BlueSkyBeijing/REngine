
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

VertexShaderOutput BloomUpVS(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    output.Pos = float4(input.Pos, 1.0f);
    output.UV = input.UV;
    
    return output;
}

Texture2D BloomLastResaultTexture : register(t0);
SamplerState BloomLastResaultTextureSampler : register(s0);

Texture2D BloomDownTexture : register(t1);
SamplerState BloomDownTextureSampler : register(s1);

float4 BloomUpPS(VertexShaderOutput input) : SV_TARGET
{
    float4 bloomLastResault = BloomLastResaultTexture.Sample(BloomLastResaultTextureSampler, input.UV);
    float4 bloomDown = BloomDownTexture.Sample(BloomDownTextureSampler, input.UV);
	
    float4 color = lerp(bloomLastResault, bloomDown, 0.5f);
    
    return color;
}
