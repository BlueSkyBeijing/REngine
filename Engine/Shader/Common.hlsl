#define PI 3.1415926

float Square(float x)
{
    return x * x;
}

float Pow2(float x)
{
    return x * x;
}

float Pow4(float x)
{
    float xx = x * x;
    return xx * xx;
}

float Pow5(float x)
{
    float xx = x * x;
    return xx * xx * x;
}

float3 RGBMDecode( float4 rgbm, float MaxValue )
{
	return rgbm.rgb * (rgbm.a * MaxValue);
}

float3 RGBMDecode( float4 rgbm )
{
	return rgbm.rgb * (rgbm.a * 64.0f);
}
