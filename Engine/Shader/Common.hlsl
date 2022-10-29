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

float3 TransformTangentVectorToWorld(float3x3 tangentToWorld, float3 inTangentVector)
{
	// Transform directly to world space
	// The vector transform is optimized for this case, only one vector-matrix multiply is needed
	return mul(inTangentVector, tangentToWorld);
}

float3 TransformTangentNormalToWorld(float3x3 tangentToWorld, float3 tangentNormal)
{
	return normalize(float3(TransformTangentVectorToWorld(tangentToWorld, tangentNormal)));
}

float3x3 CalcTangentToLocal(float3 tangentX, float4 tangentZ, inout float tangentSign)
{
    tangentSign = tangentZ.w;
	float3 tangentY = cross(tangentZ.xyz, tangentX) * tangentSign;

	float3x3 result;
	result[0] = cross(tangentY, tangentZ.xyz) * tangentSign;
	result[1] = tangentY;
	result[2] = tangentZ.xyz;

	return result;
}

float3x3 CalcTangentToWorld(float3x3 localToWorld, float3x3 tangentToLocal)
{
    // remove scaling
    localToWorld[0] = normalize(localToWorld[0]);
    localToWorld[1] = normalize(localToWorld[1]);
    localToWorld[2] = normalize(localToWorld[2]);
    half3x3 tangentToWorld = mul(tangentToLocal, localToWorld);

    return tangentToWorld;
}

/** Assemble the transform from tangent space into world space */
float3x3 AssembleTangentToWorld( float3 tangentToWorld0, float4 tangentToWorld2)
{
	// Will not be orthonormal after interpolation. This perfectly matches xNormal.
	// Any mismatch with xNormal will cause distortions for baked normal maps.

	// Derive the third basis vector off of the other two.
	// Flip based on the determinant sign
	float3 tangentToWorld1 = cross(tangentToWorld2.xyz, tangentToWorld0) * tangentToWorld2.w;
	// Transform from tangent space to world space
	return float3x3(tangentToWorld0, tangentToWorld1, tangentToWorld2.xyz);
}