

cbuffer cbObject : register(b0)
{
    float4 PosScaleBias;
	float2 InvTargetSize;
};

void DrawRectangle(in float4 InPosition, in float2 InTexCoord, out float4 OutPosition, out float2 OutTexCoord)
{
    OutPosition = InPosition;
    OutPosition.xy = -1.0f + 2.0f * (PosScaleBias.zw + (InPosition.xy * PosScaleBias.xy)) * InvTargetSize.xy;
    OutPosition.xy *= float2(1, -1);
}

void PostprocessVS(
	in float4 InPosition : ATTRIBUTE0,
	in float2 InTexCoord : ATTRIBUTE1,
	out float4 OutUVPos : TEXCOORD0,
	out float4 OutPosition : SV_POSITION
	)
{
    DrawRectangle(InPosition, InTexCoord, OutPosition, OutUVPos.xy);
    OutUVPos.zw = OutPosition.xy;
}

Texture2D PostprocessInput0 : register(t0);
SamplerState PostprocessInput0Sampler : register(s0);

void PostprocessPS(
	float4 InUVPos : TEXCOORD0,
	out half4 OutColor : SV_Target0
	)
{
    OutColor = PostprocessInput0.Sample(PostprocessInput0Sampler, InUVPos.xy);
}
