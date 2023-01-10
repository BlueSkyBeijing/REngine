
struct VertexShaderInput
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
};

struct BloomSetupVertexShaderOutput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float2 SampleUV[4] : TEXCOORD1;
};

struct VertexShaderOutput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float4 SampleUV[8] : TEXCOORD1;
};

cbuffer PostProcessingConstantBuffer : register(b0)
{
    float3 BloomColor;
    float BloomThreshold;
    float2 SceneColorInvSize;
    float2 _Placeholder0;
};

cbuffer FBloomDownConstant : register(b0)
{
    float2 BloomDownInvSize;
    float BloomDownScale;
    float _Placeholder1;
};

cbuffer FBloomUpConstant : register(b0)
{
    float4 BloomTintA;
    float4 BloomTintB;
    float2 BloomLastResaultInvSize;
    float2 BloomUpSourceInvSize;
    float2 BloomUpScales;
    float2 _Placeholder2;
};


float Luminance(float3 linearColor)
{
    return dot(linearColor, float3(0.3, 0.59, 0.11));
}

float2 Circle(float start, float Points, float Point)
{
    float rad = (3.141592 * 2.0 * (1.0 / Points)) * (Point + start);
    return float2(sin(rad), cos(rad));
}

BloomSetupVertexShaderOutput BloomSetupVS(VertexShaderInput input)
{
    BloomSetupVertexShaderOutput output;
    
    output.Pos = float4(input.Pos, 1.0f);
    output.UV = input.UV;
    
    output.SampleUV[0].xy = output.UV + SceneColorInvSize.xy * float2(-1, -1);
    output.SampleUV[1].xy = output.UV + SceneColorInvSize.xy * float2(1, -1);
    output.SampleUV[2].xy = output.UV + SceneColorInvSize.xy * float2(-1, 1);
    output.SampleUV[3].xy = output.UV + SceneColorInvSize.xy * float2(1, 1);
    
    return output;
}

Texture2D SceneColorTexture : register(t0);
SamplerState LinearClampTextureSampler : register(s2);

float4 BloomSetupPS(BloomSetupVertexShaderOutput input) : SV_TARGET
{ 
    float4 bloomSample0 = SceneColorTexture.Sample(LinearClampTextureSampler, input.SampleUV[0].xy);
    float4 bloomSample1 = SceneColorTexture.Sample(LinearClampTextureSampler, input.SampleUV[1].xy);
    float4 bloomSample2 = SceneColorTexture.Sample(LinearClampTextureSampler, input.SampleUV[2].xy);
    float4 bloomSample3 = SceneColorTexture.Sample(LinearClampTextureSampler, input.SampleUV[3].xy);
 
    float4 averageColor = 1.0f;
    averageColor.rgb = (bloomSample0.rgb * 0.25f) + (bloomSample1.rgb * 0.25f) + (bloomSample2.rgb * 0.25f) + (bloomSample3.rgb * 0.25f);

    float totalLuminance = Luminance(averageColor.rgb);
    float bloomLuminance = totalLuminance - BloomThreshold;
    float amount = saturate(bloomLuminance * 0.001f);
   
    float4 color = 1.0f;
    color.rgb = averageColor.rgb;
    color.rgb *= amount;

    return color;
}

VertexShaderOutput BloomDownVS(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    output.Pos = float4(input.Pos, 1.0f);
    output.UV = input.UV;
   
    float start = 2.0f / 14.0f;
    float scale = BloomDownScale;

    output.SampleUV[0].xy = input.UV.xy;
    output.SampleUV[0].zw = input.UV.xy + Circle(start, 14.0, 0.0) * scale * BloomDownInvSize;
    output.SampleUV[1].xy = input.UV.xy + Circle(start, 14.0, 1.0) * scale * BloomDownInvSize;
    output.SampleUV[1].zw = input.UV.xy + Circle(start, 14.0, 2.0) * scale * BloomDownInvSize;
    output.SampleUV[2].xy = input.UV.xy + Circle(start, 14.0, 3.0) * scale * BloomDownInvSize;
    output.SampleUV[2].zw = input.UV.xy + Circle(start, 14.0, 4.0) * scale * BloomDownInvSize;
    output.SampleUV[3].xy = input.UV.xy + Circle(start, 14.0, 5.0) * scale * BloomDownInvSize;
    output.SampleUV[3].zw = input.UV.xy + Circle(start, 14.0, 6.0) * scale * BloomDownInvSize;
    output.SampleUV[4].xy = input.UV.xy + Circle(start, 14.0, 7.0) * scale * BloomDownInvSize;
    output.SampleUV[4].zw = input.UV.xy + Circle(start, 14.0, 8.0) * scale * BloomDownInvSize;
    output.SampleUV[5].xy = input.UV.xy + Circle(start, 14.0, 9.0) * scale * BloomDownInvSize;
    output.SampleUV[5].zw = input.UV.xy + Circle(start, 14.0, 10.0) * scale * BloomDownInvSize;
    output.SampleUV[6].xy = input.UV.xy + Circle(start, 14.0, 11.0) * scale * BloomDownInvSize;
    output.SampleUV[6].zw = input.UV.xy + Circle(start, 14.0, 12.0) * scale * BloomDownInvSize;
    output.SampleUV[7].xy = input.UV.xy + Circle(start, 14.0, 13.0) * scale * BloomDownInvSize;
    output.SampleUV[7].zw = float2(0.0, 0.0);
    
    return output;
}

Texture2D BloomDownSource : register(t0);

float4 BloomDownPS(VertexShaderOutput input) : SV_TARGET
{
    float4 nearby0 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[0].xy).rgba;
    float4 nearby1 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[0].zw).rgba;
    float4 nearby2 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[1].xy).rgba;
    float4 nearby3 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[1].zw).rgba;
    float4 nearby4 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[2].xy).rgba;
    float4 nearby5 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[2].zw).rgba;
    float4 nearby6 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[3].xy).rgba;
    float4 nearby7 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[3].zw).rgba;
    float4 nearby8 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[4].xy).rgba;
    float4 nearby9 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[4].zw).rgba;
    float4 nearby10 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[5].xy).rgba;
    float4 nearby11 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[5].zw).rgba;
    float4 nearby12 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[6].xy).rgba;
    float4 nearby13 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[6].zw).rgba;
    float4 nearby14 = BloomDownSource.Sample(LinearClampTextureSampler, input.SampleUV[7].xy).rgba;
    
    float weight = 1.0f / 15.0f; 
    float4 color = 1.0f;
    
    color.rgba =
		(nearby0 * weight) +
		(nearby1 * weight) +
		(nearby2 * weight) +
		(nearby3 * weight) +
		(nearby4 * weight) +
		(nearby5 * weight) +
		(nearby6 * weight) +
		(nearby7 * weight) +
		(nearby8 * weight) +
		(nearby9 * weight) +
		(nearby10 * weight) +
		(nearby11 * weight) +
		(nearby12 * weight) +
		(nearby13 * weight) +
		(nearby14 * weight);

    return color;
}

VertexShaderOutput BloomUpVS(VertexShaderInput input)
{
    VertexShaderOutput output;

    output.Pos = float4(input.Pos, 1.0f);
    output.UV = input.UV;

    float start = 2.0 / 7.0;
    float scale = BloomUpScales.x;

    output.SampleUV[0].xy = input.UV.xy + Circle(start, 7.0, 0.0) * scale * BloomLastResaultInvSize;
    output.SampleUV[0].zw = input.UV.xy + Circle(start, 7.0, 1.0) * scale * BloomLastResaultInvSize;
    output.SampleUV[1].xy = input.UV.xy + Circle(start, 7.0, 2.0) * scale * BloomLastResaultInvSize;
    output.SampleUV[1].zw = input.UV.xy + Circle(start, 7.0, 3.0) * scale * BloomLastResaultInvSize;
    output.SampleUV[2].xy = input.UV.xy + Circle(start, 7.0, 4.0) * scale * BloomLastResaultInvSize;
    output.SampleUV[2].zw = input.UV.xy + Circle(start, 7.0, 5.0) * scale * BloomLastResaultInvSize;
    output.SampleUV[3].xy = input.UV.xy + Circle(start, 7.0, 6.0) * scale * BloomLastResaultInvSize;
    output.SampleUV[3].zw = input.UV.xy;

    start = 2.0 / 7.0;
    scale = BloomUpScales.y;

    output.SampleUV[4].xy = input.UV.xy + Circle(start, 7.0, 0.0) * scale * BloomUpSourceInvSize;
    output.SampleUV[4].zw = input.UV.xy + Circle(start, 7.0, 1.0) * scale * BloomUpSourceInvSize;
    output.SampleUV[5].xy = input.UV.xy + Circle(start, 7.0, 2.0) * scale * BloomUpSourceInvSize;
    output.SampleUV[5].zw = input.UV.xy + Circle(start, 7.0, 3.0) * scale * BloomUpSourceInvSize;
    output.SampleUV[6].xy = input.UV.xy + Circle(start, 7.0, 4.0) * scale * BloomUpSourceInvSize;
    output.SampleUV[6].zw = input.UV.xy + Circle(start, 7.0, 5.0) * scale * BloomUpSourceInvSize;
    output.SampleUV[7].xy = input.UV.xy + Circle(start, 7.0, 6.0) * scale * BloomUpSourceInvSize;
    output.SampleUV[7].zw = float2(0.0, 0.0);
    
    return output;
}


Texture2D BloomLastResaultTexture : register(t0);
Texture2D BloomDownTexture : register(t1);

float4 BloomUpPS(VertexShaderOutput input) : SV_TARGET
{   
    float3 aNearby0 = BloomDownTexture.Sample(LinearClampTextureSampler, input.SampleUV[0].xy).rgb;
    float3 aNearby1 = BloomDownTexture.Sample(LinearClampTextureSampler, input.SampleUV[0].zw).rgb;
    float3 aNearby2 = BloomDownTexture.Sample(LinearClampTextureSampler, input.SampleUV[1].xy).rgb;
    float3 aNearby3 = BloomDownTexture.Sample(LinearClampTextureSampler, input.SampleUV[1].zw).rgb;
    float3 aNearby4 = BloomDownTexture.Sample(LinearClampTextureSampler, input.SampleUV[2].xy).rgb;
    float3 aNearby5 = BloomDownTexture.Sample(LinearClampTextureSampler, input.SampleUV[2].zw).rgb;
    float3 aNearby6 = BloomDownTexture.Sample(LinearClampTextureSampler, input.SampleUV[3].xy).rgb;
    float3 aNearby7 = BloomDownTexture.Sample(LinearClampTextureSampler, input.SampleUV[3].zw).rgb;

    float3 bNearby0 = BloomLastResaultTexture.Sample(LinearClampTextureSampler, input.SampleUV[3].zw).rgb;
    float3 bNearby1 = BloomLastResaultTexture.Sample(LinearClampTextureSampler, input.SampleUV[4].xy).rgb;
    float3 bNearby2 = BloomLastResaultTexture.Sample(LinearClampTextureSampler, input.SampleUV[4].zw).rgb;
    float3 bNearby3 = BloomLastResaultTexture.Sample(LinearClampTextureSampler, input.SampleUV[5].xy).rgb;
    float3 bNearby4 = BloomLastResaultTexture.Sample(LinearClampTextureSampler, input.SampleUV[5].zw).rgb;
    float3 bNearby5 = BloomLastResaultTexture.Sample(LinearClampTextureSampler, input.SampleUV[6].xy).rgb;
    float3 bNearby6 = BloomLastResaultTexture.Sample(LinearClampTextureSampler, input.SampleUV[6].zw).rgb;
    float3 bNearby7 = BloomLastResaultTexture.Sample(LinearClampTextureSampler, input.SampleUV[7].xy).rgb;

    float3 weightA = BloomTintA.rgb;
    float3 weightB = BloomTintB.rgb;

    float4 color = 1.0f;
    color.rgb =
          aNearby0 * weightA +
          aNearby1 * weightA +
          aNearby2 * weightA +
          aNearby3 * weightA +
          aNearby4 * weightA +
          aNearby5 * weightA +
          aNearby6 * weightA +
          aNearby7 * weightA +
          bNearby0 * weightB +
          bNearby1 * weightB +
          bNearby2 * weightB +
          bNearby3 * weightB +
          bNearby4 * weightB +
          bNearby5 * weightB +
          bNearby6 * weightB +
          bNearby7 * weightB;
        
    return color;
}
