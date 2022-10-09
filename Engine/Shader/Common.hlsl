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
    
    float3 EmissiveColor;
    float3 SubsurfaceColor;
    
    float3 DiffuseColor;
    float3 SpecularColor;
};

void InitMaterialContext(inout MaterialContext context,
    float3 baseColor,
    float metallic,
    float specular,
    float roughness,
    float opacity,

    float3 emissiveColor,
    float3 subsurfaceColor
)
{
    context.BaseColor = baseColor;
    context.Metallic = metallic;
    context.Specular = specular;
    context.Roughness = roughness;
    context.Opacity = opacity;

    context.EmissiveColor = emissiveColor;
    context.SubsurfaceColor = subsurfaceColor;
    context.DiffuseColor = baseColor;
    context.SpecularColor = float3(0,0,0);
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
    float Shadow;
    float Thickness; 
};

void InitLightingContext(inout LightingContext context,
    float3 lightDir,
    float lightIntensity,
    float3 lightColor,
    float3 normal,
    float3 viewDir,
    float3 cameraPos,
    float3 lightPos = float3(0.0f, 0.0f, 0.0f),
    float lightRadius = 10.0f,
    float3 pixelPos = float3(0.0f, 0.0f, 0.0f),
    float shadow = 1.0f,
    float thickness = 1.0f
)
{
    context.LightDir = lightDir;
    context.LightIntensity = lightIntensity;
    context.LightColor = lightColor;
    context.Normal = normal;
    context.ViewDir = viewDir;
    context.CameraPos = cameraPos;
    context.LightPos = lightPos;
    context.LightRadius = lightRadius;
    context.PixelPos = pixelPos;
    context.Shadow = shadow;
    context.Thickness = thickness;
}
