
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

VertexShaderOutput PostprocessVS(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    output.Pos = float4(input.Pos, 1.0f);
    output.UV = input.UV;
    
    return output;
}

Texture2D FullScreenTexture : register(t0);
SamplerState FullScreenTextureSampler : register(s0);

float4 PostprocessPS(VertexShaderOutput input) : SV_TARGET
{
    float4 color = FullScreenTexture.Sample(FullScreenTextureSampler, input.UV);
	
    return color;
}
