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

struct MaterialContext
{
    float3 BaseColor;
    float Metallic;
    float Specular;
    float Roughness;
    float Opacity;
};

void InitMaterialContext(inout MaterialContext context,
    float3 baseColor,
    float metallic,
    float specular,
    float roughness,
    float opacity
)
{
    context.BaseColor = baseColor;
    context.Metallic = metallic;
    context.Specular = specular;
    context.Roughness = roughness;
    context.Opacity = opacity;
}

struct LightingContext
{
    float3 LightDir;
    float LightIntensity;
    float3 LightColor;
    float3 Normal;
    float3 ViewDir;
    float3 CameraPos;
    float3 LightPos;
    float LightRadius;
    float3 PixelPos;
};

void InitLightingContext(inout LightingContext context,
    float3 LightDir,
    float LightIntensity,
    float3 LightColor,
    float3 Normal,
    float3 ViewDir,
    float3 CameraPos,
    float3 LightPos = float3(0.0f, 0.0f, 0.0f),
    float LightRadius = 10,
    float3 PixelPos = float3(0.0f, 0.0f, 0.0f)
)
{
    context.LightDir = LightDir;
    context.LightIntensity = LightIntensity;
    context.LightColor = LightColor;
    context.Normal = Normal;
    context.ViewDir = ViewDir;
    context.CameraPos = CameraPos;
    context.LightPos = LightPos;
    context.LightRadius = LightRadius;
    context.PixelPos = PixelPos;
}
