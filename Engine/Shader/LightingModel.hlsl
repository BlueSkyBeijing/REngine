#include "Common.hlsl"

#define SHADING_MODEL_UNLIT 0
#define SHADING_MODEL_DEFAULT_LIT 1
#define SHADING_MODEL_SUBSUFACE 2
#define SHADING_MODEL_PREINTEGRATED_SKIN 3
#define SHADING_MODEL_CLEAR_COAT 4
#define SHADING_MODEL_TWO_SIDE_FOLIAGE 5
#define SHADING_MODEL_HAIR 6
#define SHADING_MODEL_CLOTH 7
#define SHADING_MODEL_EYE 8
#define SHADING_MODEL_SIGLE_LAYER_WATER 9
#define SHADING_MODEL_THIN_TRANSLUCENT 10
#define SHADING_MODEL_STRATA 11


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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float DielectricSpecularToF0(float specular)
{
	return 0.08f * specular;
}

float3 ComputeF0(float specular, float3 baseColor, float metallic)
{
	return lerp(DielectricSpecularToF0(specular).xxx, baseColor, metallic.xxx);
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

void Init(inout BxDFContext context, half3 normal, half3 viewDir, half3 lightDir)
{
    context.NoL = dot(normal, lightDir);
    context.NoV = dot(normal, viewDir);
    context.VoL = dot(viewDir, lightDir);
    float InvLenH = rsqrt(2 + 2 * context.VoL);
    context.NoH = saturate((context.NoL + context.NoV) * InvLenH);
    context.VoH = saturate(InvLenH + InvLenH * context.VoL);
	//NoL = saturate( NoL );
	//NoV = saturate( abs( NoV ) + 1e-5 );

    context.XoV = 0.0f;
    context.XoL = 0.0f;
    context.XoH = 0.0f;
    context.YoV = 0.0f;
    context.YoL = 0.0f;
    context.YoH = 0.0f;
}

float3 Diffuse_Lambert(float3 diffuseColor)
{
    return diffuseColor * (1 / PI);
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
float3 F_Schlick(float3 specularColor, float VoH)
{
    float Fc = Pow5(1 - VoH); // 1 sub, 3 mul
	//return Fc + (1 - Fc) * specularColor;		// 1 add, 3 mad
	
	// Anything less than 2% is physically impossible and is instead considered to be shadowing
    return saturate(50.0 * specularColor.g) * Fc + (1 - Fc) * specularColor;
}

float3 SpecularGGX(float roughness, float3 specularColor, BxDFContext context, float NoL)
{
    float a2 = Pow4(roughness);
	
	// Generalized microfacet specular
    float D = D_GGX(a2, context.NoH);
    float Vis = Vis_SmithJointApprox(a2, context.NoV, NoL);
    float3 F = F_Schlick(specularColor, context.VoH);

    return (D * Vis) * F;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 Unlit(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float shadow)
{    
    return diffuseColor;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 DefaultLitBxDF(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float roughness, float3 specularColor, float shadow)
{
    lightColor *= lightIntensity;
    
    BxDFContext context;
    Init(context, normal, viewDir, lightDir);

    float NoL = max(dot(lightDir, normal), 0.0);

    float3 diffuse = NoL * Diffuse_Lambert(diffuseColor) * lightColor;
    float3 specular = NoL * SpecularGGX(roughness, specularColor, context, NoL) * lightColor;
    
    return (diffuse + specular) * shadow;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 SubsurfaceBxDF(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float shadow)
{
    float roughness = 0.5f;
    float specular = 0.5f;
    float metallic = 0.5f;
    float3 specularColor = ComputeF0(specular, diffuseColor, metallic);

    float3 lighting = DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
	
    float3 subsurfaceColor = float3(1, 0, 0);
    float opacity = 0.9;

    float3 h = normalize(viewDir + lightDir);

	// to get an effect when you see through the material
	// hard coded pow constant
    float inScatter = pow(saturate(dot(lightDir, -viewDir)), 12) * lerp(3, .1f, opacity);
	// wrap around lighting, /(PI*2) to be energy consistent (hack do get some view dependnt and light dependent effect)
	// Opacity of 0 gives no normal dependent lighting, Opacity of 1 gives strong normal contribution
    float normalContribution = saturate(dot(normal, h) * opacity + 1 - opacity);
    float backScatter = normalContribution / (PI * 2);
	
	// lerp to never exceed 1 (energy conserving)
    float3 transmission = lerp(backScatter, 1, inScatter) * subsurfaceColor;

    return lighting + transmission;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 CalcThinTransmission(float NoL, float NoV, float3 BaseColor, float Metallic)
{
    float3 transmission = 1.0;

    float AbsorptionMix = Metallic;
    if (AbsorptionMix > 0.0)
    {
        float LayerThickness = 1.0; // Assume normalized thickness
        float ThinDistance = LayerThickness * (rcp(NoV) + rcp(NoL));

		// Base color represents reflected color viewed at 0 incidence angle, after being absorbed through the substrate.
		// Because of this, extinction is normalized by traveling through layer thickness twice
        float3 TransmissionColor = Diffuse_Lambert(BaseColor);
        float3 ExtinctionCoefficient = -log(TransmissionColor) / (2.0 * LayerThickness);
        float3 OpticalDepth = ExtinctionCoefficient * max(ThinDistance - 2.0 * LayerThickness, 0.0);
        transmission = exp(-OpticalDepth);
        transmission = lerp(1.0, transmission, AbsorptionMix);
    }
    return transmission;
}

float RefractBlendClearCoatApprox(float VoH)
{
	// Polynomial approximation of refraction blend factor for normal component of VoH with fixed Eta (1/1.5):
    return (0.63 - 0.22 * VoH) * VoH - 0.745;
}

BxDFContext RefractClearCoatContext(BxDFContext context)
{
	// Reference: Propagation of refraction through dot-product NoV
	// Note: This version of Refract requires viewDir to point away from the point of incidence
	//  NoV2 = -dot(normal, Refract(viewDir, H, Eta))
	//  NoV2 = -dot(normal, RefractBlend(VoH, Eta) * H - Eta * viewDir)
	//  NoV2 = -(RefractBlend(VoH, Eta) * NoH - Eta * NoV)
	//  NoV2 = Eta * NoV - RefractBlend(VoH, Eta) * NoH
	//  NoV2 = 1.0 / 1.5 * NoV - RefractBlendClearCoatApprox(VoH) * NoH

    BxDFContext RefractedContext = context;
    float Eta = 1.0 / 1.5;
    float RefractionBlendFactor = RefractBlendClearCoatApprox(context.VoH);
    float RefractionProjectionTerm = RefractionBlendFactor * context.NoH;
    RefractedContext.NoV = clamp(Eta * context.NoV - RefractionProjectionTerm, 0.001, 1.0); // Due to CalcThinTransmission and Vis_SmithJointAniso, we need to make sure
    RefractedContext.NoL = clamp(Eta * context.NoL - RefractionProjectionTerm, 0.001, 1.0); // those values are not 0s to avoid NaNs.
    RefractedContext.VoH = saturate(Eta * context.VoH - RefractionBlendFactor);
    RefractedContext.VoL = 2.0 * RefractedContext.VoH * RefractedContext.VoH - 1.0;
    RefractedContext.NoH = context.NoH;
    return RefractedContext;
}

float3 ClearCoatBxDF(half3 normal, half3 viewDir, half3 lightDir,float3 fuzzColor, float cloth, float roughness, float3 diffuseColor, float3 specularColor)
{
    float NoL = max(dot(lightDir, normal), 0.0);

    const float clearCoat = 0.1f;
    const float clearCoatRoughness = max(0.1, 0.02f);
    const float film = 1 * clearCoat;
    const float metalSpec = 0.9;
    float3 lighting;
    BxDFContext context;
    half3 nspec = normal;

    half3 X = 0;
    half3 Y = 0;

	//////////////////////////////
	/// Top Layer
	//////////////////////////////

	// No anisotropy for the top layer
    Init(context, nspec, viewDir, lightDir);
	
	// Modify sphereSinAlpha, knowing that it was previously manipulated by roughness of the under coat
	// Note: the operation is not invertible for GBuffer.roughness = 1.0, so roughness is clamped to 254.0/255.0
    float sphereSinAlpha = 0.1f;
    float roughnessCompensation = 1 - Pow2(roughness);
    float alpha = Pow2(clearCoatRoughness);
    roughnessCompensation = roughnessCompensation > 0.0 ? (1 - alpha) / roughnessCompensation : 0.0;

    context.NoV = saturate(abs(context.NoV) + 1e-5);

	// Hard-coded Fresnel evaluation with IOR = 1.5 (for polyurethane cited by Disney BRDF)
    float F0 = 0.04;
    float Fc = Pow5(1 - context.VoH);
    float F = Fc + (1 - Fc) * F0;

	// Generalized microfacet specular
    float a2 = Pow2(alpha);
    float D = D_GGX(a2, context.NoH);
    float Vis = Vis_SmithJointApprox(a2, context.NoV, NoL);

    float Fr1 = D * Vis * F;
    float3 specular = clearCoat * (NoL * Fr1);

	// Restore previously changed sphereSinAlpha for the top layer. 
	// alpha needs to also be restored to the bottom layer roughness.
    alpha = Pow2(roughness);

	// Incoming and exiting Fresnel terms are identical to incoming Fresnel term (VoH == HoL)
	// float fresnelCoeff = (1.0 - F1) * (1.0 - F2);
	// Preserve old behavior when energy conservation is disabled
    float fresnelCoeff = 1.0 - F;
    fresnelCoeff *= fresnelCoeff;

	//////////////////////////////
	/// Bottom Layer
	//////////////////////////////

	// Propagate refraction through dot-products rather than the original vectors:
	// Reference:
	//   float Eta = 1.0 / 1.5;
	//   float3 H = normalize(viewDir + lightDir);
	//   float3 V2 = Refract(viewDir, H, Eta);
	//   float3 L2 = reflect(V2, H);
	//   V2 = -V2;
	//   BxDFContext BottomContext;
	//   Init(BottomContext, normal, X, Y, V2, L2);
    BxDFContext BottomContext = RefractClearCoatContext(context);


	// Absorption
	// Default Lit
    float3 defaultDiffuse = (NoL) *  Diffuse_Lambert(diffuseColor);
    float3 refractedDiffuse = fresnelCoeff * defaultDiffuse;
    float3 diffuse = lerp(defaultDiffuse, refractedDiffuse, clearCoat);


    a2 = Pow4(roughness);
    float D2 = 0;
    float vis2 = 0;


    D2 = D_GGX(a2, BottomContext.NoH);
	// NoL is chosen to provide better parity with DefaultLit when clearCoat=0
    vis2 = Vis_SmithJointApprox(a2, BottomContext.NoV, NoL);
    float3 F_DefaultLit = F_Schlick(specularColor, context.VoH);
		

	// Note: reusing D and viewDir from refracted context to save computation when clearCoat < 1
    float3 commonSpecular = (NoL * D2 * vis2);
    float3 defaultSpecular = F_DefaultLit;
    float3 refractedSpecular = fresnelCoeff;
    specular += commonSpecular * lerp(defaultSpecular, refractedSpecular, clearCoat);
    lighting = diffuse + specular;
    return lighting;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

float3 ClothBxDF(float3 normal, float3 viewDir, float3 lightDir, float3 fuzzColor, float cloth, float roughness, float3 diffuseColor, float3 specularColor)
{
    float NoL = max(dot(lightDir, normal), 0.0);

    BxDFContext context;
    Init(context, normal, viewDir, lightDir);
    context.NoV = saturate(abs(context.NoV) + 1e-5);

    float3 spec1 = NoL * SpecularGGX(roughness, specularColor, context, NoL);

	// cloth - Asperity Scattering - Inverse Beckmann Layer
    float d2 = D_InvGGX(Pow4(roughness), context.NoH);
    float vis2 = Vis_Cloth(context.NoV, NoL);
    float3 f2 = F_Schlick(fuzzColor, context.VoH);
    float3 spec2 = NoL * (d2 * vis2) * f2;
	
    float3 diffuse = NoL * Diffuse_Lambert(diffuseColor);
    float3 specular = lerp(spec1, spec2, cloth);

    return diffuse + specular;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 OctahedronToUnitVector( float2 oct )
{
	float3 normal = float3( oct, 1 - dot( 1, abs(oct) ) );
	float t = max( -normal.z, 0 );
	normal.xy += normal.xy >= 0 ? float2(-t, -t) : float2(t, t);
	return normalize(normal);
}

float3 EyeBxDF(half3 normal, half3 viewDir, half3 lightDir, float specular, float3 specularColor, float3 diffuseColor, float roughness, float shadow )
{
    float NoL = max(dot(lightDir, normal), 0.0);

    float2 oct = 1;
    float3 irisNormal		= OctahedronToUnitVector(oct);
	const float  irisDistance	= 1;
	const float  irisMask		= 1;

	// Blend in the negative intersection normal to create some concavity
	// Not great as it ties the concavity to the convexity of the cornea surface
	// No good justification for that. On the other hand, if we're just looking to
	// introduce some concavity, this does the job.
	const float3 causticNormal = normalize(lerp(irisNormal, -normal, irisMask*irisDistance));


	BxDFContext context;
	Init( context, normal, viewDir, lightDir );

	context.NoV = saturate( abs( context.NoV ) + 1e-5 );
	
	// F_Schlick
	float F0 = specular * 0.08;
	float Fc = Pow5( 1 - context.VoH );
	float F = Fc + (1 - Fc) * F0;
	
	float a2 = Pow4( roughness );

	// Generalized microfacet specular
	float D = D_GGX( a2, context.NoH );
	float Vis = Vis_SmithJointApprox( a2, context.NoV, NoL );
		
	float3 specularLighting = specularColor * NoL * D * Vis * F;

	float irisNoL = saturate( dot( irisNormal, lightDir ) );
	float power = lerp( 12, 1, irisNoL );
	float caustic = 0.8 + 0.2 * ( power + 1 ) * pow( saturate( dot( causticNormal, lightDir ) ), power );
	float iris = irisNoL * caustic;
	float sclera = NoL;
	
	// Preserve old behavior when energy conservation is disabled
	const float energyPreservation = 1.0f - F;
	float3 diffuse = 0;
	float3 transmission = (lerp( sclera, iris, irisMask ) * energyPreservation ) * Diffuse_Lambert(diffuseColor );
	float3 lighting = diffuse + specularLighting + transmission;
    
    return lighting;
    
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 Lighting(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float roughness, float specular, float metallic, float shadow)
{
    float3 specularColor = ComputeF0(specular, diffuseColor, metallic);
    diffuseColor = diffuseColor - diffuseColor * metallic;
    
#if SHADING_MODEL == SHADING_MODEL_UNLIT
    return Unlit(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_DEFAULT_LIT
    return DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_SUBSUFACE
    return SubsurfaceBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_PREINTEGRATED_SKIN
    return SubsurfaceBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_CLEAR_COAT
    float cloth = 0.5;
    float3 fuzzColor = float3(1, 0, 0);
    return ClearCoatBxDF(normal, viewDir, lightDir, fuzzColor, cloth, roughness, diffuseColor, specularColor);
#elif SHADING_MODEL == SHADING_MODEL_TWO_SIDE_FOLIAGE
    return SubsurfaceBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_HAIR
    return DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_CLOTH
    float3 fuzzColor = float3(1, 0, 0);
    float cloth = 1;
    return ClothBxDF(normal, viewDir lightDir, fuzzColor, cloth, roughness, diffuseColor, specularColor);
#elif SHADING_MODEL == SHADING_MODEL_EYE
    return EyeBxDF(normal, viewDir, lightDir, specular, specularColor, diffuseColor, roughness, shadow);
#elif SHADING_MODEL == SHADING_MODEL_SIGLE_LAYER_WATER
    return DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_THIN_TRANSLUCENT
    return DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_STRATA
    return DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
#else
    return BlinnPhong(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, shadow);
#endif
}

float3 DirectionalLighting(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float shadow, float thickness)
{
    //return float3(0,0,0);
    float roughness = 0.5f;
    float specular = 0.5f;
    float metallic = 0.5f;
    float3 lighting = Lighting(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specular, metallic, shadow);
    
    #if SHADING_MODEL == SHADING_MODEL_SUBSUFACE
    lighting *= thickness;
    #endif
    
    return  lighting;
}

float3 PointLighting(float3 normal, float3 lightPosition, float3 lightColor, float lightIntensity, float invRadius, float3 pixelPosition, float3 viewDir, float3 diffuseColor, float shadow)
{
    float roughness = 0.5f;
    float specular = 0.5f;
    float metallic = 0.5f;
    
    float3 toLight = lightPosition - pixelPosition;
    float distanceSqr = dot(toLight, toLight);
    float3 lightDir = toLight * rsqrt(distanceSqr);
    
    float lightIntensityPixel = saturate(1 - (distanceSqr * invRadius * invRadius)) * lightIntensity;
    return Lighting(normal, lightDir, lightColor, lightIntensityPixel, viewDir, diffuseColor, roughness, specular, metallic, 1);
}
