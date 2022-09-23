#include "Common.hlsl"

// Blinn-Phong
// from: https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model
float3 BlinnPhong(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float shadow)
{
    const float3 ambientColor = float3(0.3f, 0.3f, 0.3f);
    const float3 specularColor = float3(1.0f, 1.0f, 1.0f);
    const float shininess = 16.0;
    
    float lambertian = max(dot(lightDir, normal), 0.0);
    float3 halfDir = normalize(lightDir + viewDir);
    float specAngle = max(dot(halfDir, normal), 0.0);
    float specular = pow(specAngle, shininess);
    
    float3 colorLinear = ambientColor * diffuseColor +
                     diffuseColor * lambertian * lightColor * lightIntensity * shadow +
                     specularColor * specular * lightColor * lightIntensity * shadow;
    	 
    return colorLinear;
}

#define PI 3.1415926

float Square(float x)
{
    return x * x;
}

float3 PointLighting(float3 normal, float3 lightPosition, float3 lightColor, float invRadius, float3 pixelPosition, float3 diffuseColor)
{
    float3 toLight = lightPosition - pixelPosition;
    float distanceSqr = dot(toLight, toLight);
    float3 l = toLight * rsqrt(distanceSqr);
    float pointNoL = max(0, dot(normal, l));

    float attenuation = 1 / (sqrt(distanceSqr) + 1);

    float lightRadiusMask = Square(saturate(1 - Square(distanceSqr * (invRadius * invRadius))));
    attenuation *= lightRadiusMask * 100.0f;
    return (attenuation * pointNoL) * lightColor * (1.0 / PI) * diffuseColor;
}

struct BxDFContext
{
    float NoV;
    float NoL;
    float VoL;
    float NoH;
    float VoH;
    float XoV;
    float XoL;
    float XoH;
    float YoV;
    float YoL;
    float YoH;
};

void Init(inout BxDFContext Context, half3 N, half3 V, half3 L)
{
    Context.NoL = dot(N, L);
    Context.NoV = dot(N, V);
    Context.VoL = dot(V, L);
    float InvLenH = rsqrt(2 + 2 * Context.VoL);
    Context.NoH = saturate((Context.NoL + Context.NoV) * InvLenH);
    Context.VoH = saturate(InvLenH + InvLenH * Context.VoL);
	//NoL = saturate( NoL );
	//NoV = saturate( abs( NoV ) + 1e-5 );

    Context.XoV = 0.0f;
    Context.XoL = 0.0f;
    Context.XoH = 0.0f;
    Context.YoV = 0.0f;
    Context.YoL = 0.0f;
    Context.YoH = 0.0f;
}


float3 Diffuse_Lambert(float3 DiffuseColor)
{
    return DiffuseColor * (1 / PI);
}

// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX(float a2, float NoH)
{
    float d = (NoH * a2 - NoH) * NoH + 1; // 2 mad
    return a2 / (PI * d * d); // 4 mul, 1 rcp
}

// Appoximation of joint Smith term for GGX
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJointApprox(float a2, float NoV, float NoL)
{
    float a = sqrt(a2);
    float Vis_SmithV = NoL * (NoV * (1 - a) + a);
    float Vis_SmithL = NoV * (NoL * (1 - a) + a);
    return 0.5 * rcp(Vis_SmithV + Vis_SmithL);
}

// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
float3 F_Schlick(float3 SpecularColor, float VoH)
{
    float Fc = Pow5(1 - VoH); // 1 sub, 3 mul
	//return Fc + (1 - Fc) * SpecularColor;		// 1 add, 3 mad
	
	// Anything less than 2% is physically impossible and is instead considered to be shadowing
    return saturate(50.0 * SpecularColor.g) * Fc + (1 - Fc) * SpecularColor;
}

float3 SpecularGGX(float Roughness, float3 SpecularColor, BxDFContext Context, float NoL)
{
    float a2 = Pow4(Roughness);
	
	// Generalized microfacet specular
    float D = D_GGX(a2, Context.NoH);
    float Vis = Vis_SmithJointApprox(a2, Context.NoV, NoL);
    float3 F = F_Schlick(SpecularColor, Context.VoH);

    return (D * Vis) * F;
}

float3 DefaultLitBxDF(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float shadow)
{
    BxDFContext Context;
    Init(Context, normal, viewDir, lightDir);


    float NoL = max(dot(lightDir, normal), 0.0);

    float diffuse = NoL * Diffuse_Lambert(diffuseColor);
    float specular = NoL * SpecularGGX(0.5, lightColor, Context, NoL);
    
    return (diffuse + specular) * shadow;

}


float D_InvGGX(float a2, float NoH)
{
    float A = 4;
    float d = (NoH - a2 * NoH) * NoH + a2;
    return rcp(PI * (1 + A * a2)) * (1 + 4 * a2 * a2 / (d * d));
}


float Vis_Cloth(float NoV, float NoL)
{
    return rcp(4 * (NoL + NoV - NoL * NoV));
}

float3 ClothBxDF(half3 N, half3 V, half3 L, float Falloff, float NoL, float3 FuzzColor, float Cloth, float Roughness, float3 DiffuseColor, float3 SpecularColor)
{

    BxDFContext Context;
    Init(Context, N, V, L);
    Context.NoV = saturate(abs(Context.NoV) + 1e-5);

    float3 Spec1 = NoL * SpecularGGX(Roughness, SpecularColor, Context, NoL);

	// Cloth - Asperity Scattering - Inverse Beckmann Layer
    float D2 = D_InvGGX(Pow4(Roughness), Context.NoH);
    float Vis2 = Vis_Cloth(Context.NoV, NoL);
    float3 F2 = F_Schlick(FuzzColor, Context.VoH);
    float3 Spec2 = (Falloff * NoL) * (D2 * Vis2) * F2;
	
    float3 Diffuse = NoL * Diffuse_Lambert(DiffuseColor);
    float3 Specular = lerp(Spec1, Spec2, Cloth);

    return Diffuse + Specular;
}

float3 SubsurfaceBxDF(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float shadow)
{
    float3 Lighting = DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, shadow);
	
    float3 SubsurfaceColor = float3(1, 0, 0);
    float Opacity = 0.9;

    float3 H = normalize(viewDir + lightDir);

	// to get an effect when you see through the material
	// hard coded pow constant
    float InScatter = pow(saturate(dot(lightDir, -viewDir)), 12) * lerp(3, .1f, Opacity);
	// wrap around lighting, /(PI*2) to be energy consistent (hack do get some view dependnt and light dependent effect)
	// Opacity of 0 gives no normal dependent lighting, Opacity of 1 gives strong normal contribution
    float NormalContribution = saturate(dot(normal, H) * Opacity + 1 - Opacity);
    float BackScatter = NormalContribution / (PI * 2);
	
	// lerp to never exceed 1 (energy conserving)
    float Transmission = lerp(BackScatter, 1, InScatter) * SubsurfaceColor;

    return Lighting + Transmission;
}

float3 Diectional_Lighting(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float shadow)
{
#if SHADING_MODEL == 1
    return DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, shadow);
#else 
    return SubsurfaceBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, shadow);
#endif
}
