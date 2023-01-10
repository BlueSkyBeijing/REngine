#include "Common.hlsl"

#define SHADING_MODEL_UNLIT 0
#define SHADING_MODEL_DEFAULT_LIT 1
#define SHADING_MODEL_SUBSUFACE 2
#define SHADING_MODEL_PREINTEGRATED_SKIN 3
#define SHADING_MODEL_CLEAR_COAT 4
#define SHADING_MODEL_SUBSUFACE_PROFILE 5
#define SHADING_MODEL_TWO_SIDE_FOLIAGE 6
#define SHADING_MODEL_HAIR 7
#define SHADING_MODEL_CLOTH 8
#define SHADING_MODEL_EYE 9
#define SHADING_MODEL_SIGLE_LAYER_WATER 10
#define SHADING_MODEL_THIN_TRANSLUCENT 11
#define SHADING_MODEL_STRATA 12

#define REFLECTION_CAPTURE_ROUGHEST_MIP 1
#define REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE 1.2


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

void Init(inout BxDFContext context, float3 normal, float3 viewDir, float3 lightDir)
{
    context.NoL = dot(normal, lightDir);
    context.NoV = dot(normal, viewDir);
    context.VoL = dot(viewDir, lightDir);
    float InvLenH = rsqrt(2 + 2 * context.VoL);
    context.NoH = saturate((context.NoL + context.NoV) * InvLenH);
    context.VoH = saturate(InvLenH + InvLenH * context.VoL);

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
    return saturate(0.5 * rcp(Vis_SmithV + Vis_SmithL));
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
float3 Unlit(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 emissiveColor, float shadow)
{    
    return emissiveColor;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 DefaultLitBxDF(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float roughness, float3 specularColor, float shadow)
{
    BxDFContext context;
    Init(context, normal, viewDir, lightDir);

    float NoL = max(dot(lightDir, normal), 0.00001);

    float3 diffuse = NoL * Diffuse_Lambert(diffuseColor);
    float3 specular = NoL * SpecularGGX(roughness, specularColor, context, NoL);

    float3 lighting = (diffuse + specular)  * lightColor * lightIntensity * shadow;
    
    return lighting;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 SubsurfaceBxDF(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float3 specularColor, float roughness, float opacity, float3 subsurfaceColor, float shadow, float thickness)
{
    float3 lighting = DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
	
    float3 h = normalize(viewDir + lightDir);

	// to get an effect when you see through the material
	// hard coded pow constant
    float inScatter = pow(saturate(dot(lightDir, -viewDir)), 12) * lerp(3, .1f, opacity);
	// wrap around lighting, /(PI*2) to be energy consistent (hack do get some view dependnt and light dependent effect)
	// Opacity of 0 gives no normal dependent lighting, Opacity of 1 gives strong normal contribution
    float normalContribution = saturate(dot(normal, h) * opacity + 1 - opacity);
    float backScatter = normalContribution / (PI * 2);
	
	// lerp to never exceed 1 (energy conserving)
    float3 transmission = lightColor * lightIntensity * lerp(backScatter, 1, inScatter) * subsurfaceColor * thickness;
    
    lighting = lighting + transmission;

    return lighting;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 CalcThinTransmission(float NoL, float NoV, float3 baseColor, float metallic)
{
    float3 transmission = 1.0;

    float absorptionMix = metallic;
    if (absorptionMix > 0.0)
    {
        float layerThickness = 1.0; // Assume normalized thickness
        float thinDistance = layerThickness * (rcp(NoV) + rcp(NoL));

		// Base color represents reflected color viewed at 0 incidence angle, after being absorbed through the substrate.
		// Because of this, extinction is normalized by traveling through layer thickness twice
        float3 transmissionColor = Diffuse_Lambert(baseColor);
        //log 0 is inf, must clamp here
        transmissionColor = max(transmissionColor, 0.00001f);
        float3 extinctionCoefficient = -log(transmissionColor) / (2.0 * layerThickness);
        float3 opticalDepth = extinctionCoefficient * max(thinDistance - 2.0 * layerThickness, 0.0);
        transmission = exp(-opticalDepth);
        transmission = lerp(1.0, transmission, absorptionMix);
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

    BxDFContext refractedContext = context;
    float eta = 1.0 / 1.5;
    float refractionBlendFactor = RefractBlendClearCoatApprox(context.VoH);
    float refractionProjectionTerm = refractionBlendFactor * context.NoH;
    refractedContext.NoV = clamp(eta * context.NoV - refractionProjectionTerm, 0.001, 1.0); // Due to CalcThinTransmission and Vis_SmithJointAniso, we need to make sure
    refractedContext.NoL = clamp(eta * context.NoL - refractionProjectionTerm, 0.001, 1.0); // those values are not 0s to avoid NaNs.
    refractedContext.VoH = saturate(eta * context.VoH - refractionBlendFactor);
    refractedContext.VoL = 2.0 * refractedContext.VoH * refractedContext.VoH - 1.0;
    refractedContext.NoH = context.NoH;
    return refractedContext;
}

float3 ClearCoatBxDF(float3 normal, float3 viewDir, float3 lightDir, float3 lightColor, float lightIntensity, float clearCoat, float clearCoatRoughness, float roughness, float metallic, float3 diffuseColor, float3 specularColor, float shadow)
{
    float NoL = max(dot(lightDir, normal), 0.00001);

    clearCoatRoughness = max(clearCoatRoughness, 0.02f);
    const float film = 1 * clearCoat;
    const float metalSpec = 0.9;
    float3 lighting;
    BxDFContext context;
    float3 nspec = normal;

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
    float f0 = 0.04;
    float fc = Pow5(1 - context.VoH);
    float f = fc + (1 - fc) * f0;

	// Generalized microfacet specular
    float a2 = Pow2(alpha);
    float d = D_GGX(a2, context.NoH);
    float vis = Vis_SmithJointApprox(a2, context.NoV, NoL);

    float fr1 = d * vis * f;
    float3 specular = clearCoat * (NoL * fr1);

	// Restore previously changed sphereSinAlpha for the top layer. 
	// alpha needs to also be restored to the bottom layer roughness.
    alpha = Pow2(roughness);

	// Incoming and exiting Fresnel terms are identical to incoming Fresnel term (VoH == HoL)
	// float fresnelCoeff = (1.0 - F1) * (1.0 - F2);
	// Preserve old behavior when energy conservation is disabled
    float fresnelCoeff = 1.0 - f;
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
    BxDFContext bottomContext = RefractClearCoatContext(context);

	// Absorption
	float3 transmission = CalcThinTransmission(bottomContext.NoL, bottomContext.NoV, diffuseColor, metallic);

	// Default Lit
    float3 defaultDiffuse = (NoL) *  Diffuse_Lambert(diffuseColor);
    float3 refractedDiffuse = fresnelCoeff * transmission * defaultDiffuse;
    float3 diffuse = lerp(defaultDiffuse, refractedDiffuse, clearCoat);

    a2 = Pow4(roughness);
    float d2 = 0;
    float vis2 = 0;

    d2 = D_GGX(a2, bottomContext.NoH);
	// NoL is chosen to provide better parity with DefaultLit when clearCoat=0
    vis2 = Vis_SmithJointApprox(a2, bottomContext.NoV, NoL);
    float3 f_DefaultLit = F_Schlick(specularColor, context.VoH);
		
	// Note: reusing D and viewDir from refracted context to save computation when clearCoat < 1
    float3 commonSpecular = (NoL * d2 * vis2);
    float3 defaultSpecular = f_DefaultLit;
    float3 refractedSpecular = fresnelCoeff * transmission * f;
    specular += commonSpecular * lerp(defaultSpecular, refractedSpecular, clearCoat);
    
    lighting = (diffuse + specular) * lightColor * lightIntensity * shadow;
    
    return lighting;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 TwoSidedBxDF(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float roughness, float3 specularColor, float3 subsurfaceColor, float shadow)
{
    float3 lighting = DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
	
	// http://blog.stevemcauley.com/2011/12/03/energy-conserving-wrapped-diffuse/
	float wrap = 0.5;
	float wrapNoL = saturate( ( -dot(normal, lightDir) + wrap ) / Square( 1 + wrap ) );

	// Scatter distribution
	float VoL = dot(viewDir, lightDir);
	float scatter = D_GGX( 0.6*0.6, saturate( -VoL ) );

	float3 transmission = lightColor * lightIntensity * (wrapNoL * scatter) * subsurfaceColor;

    lighting = lighting + transmission * shadow;
    
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

float3 ClothBxDF(float3 normal, float3 viewDir, float3 lightDir, float3 lightColor, float lightIntensity, float3 fuzzColor, float cloth, float roughness, float3 diffuseColor, float3 specularColor, float shadow)
{
    float NoL = max(dot(lightDir, normal), 0.00001);

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
    float3 lighting = (diffuse + specular) * lightColor * lightIntensity * shadow;
    
    return lighting;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 OctahedronToUnitVector( float2 oct )
{
	float3 normal = float3( oct, 1 - dot( 1, abs(oct) ) );
	float t = max( -normal.z, 0 );
	normal.xy += normal.xy >= 0 ? float2(-t, -t) : float2(t, t);
	return normalize(normal);
}

float3 EyeBxDF(float3 normal, float3 viewDir, float3 lightDir, float3 lightColor, float lightIntensity, float specular, float3 specularColor, float3 diffuseColor, float roughness, float shadow )
{
    float NoL = max(dot(lightDir, normal), 0.00001);

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
	float f0 = specular * 0.08;
	float fc = Pow5( 1 - context.VoH );
	float f = fc + (1 - fc) * f0;
	
	float a2 = Pow4( roughness );

	// Generalized microfacet specular
	float d = D_GGX( a2, context.NoH );
	float vis = Vis_SmithJointApprox( a2, context.NoV, NoL );
		
	float3 specularLighting = specularColor * NoL * d * vis * f;

	float irisNoL = saturate( dot( irisNormal, lightDir ) );
	float power = lerp( 12, 1, irisNoL );
	float caustic = 0.8 + 0.2 * ( power + 1 ) * pow( saturate( dot( causticNormal, lightDir ) ), power );
	float iris = irisNoL * caustic;
	float sclera = NoL;
	
	// Preserve old behavior when energy conservation is disabled
	const float energyPreservation = 1.0f - f;
	float3 diffuse = 0;
	float3 transmission = (lerp( sclera, iris, irisMask ) * energyPreservation ) * Diffuse_Lambert(diffuseColor );
	float3 lighting = diffuse + specularLighting + transmission;
    lighting *= lightColor * lightIntensity;
   
    return lighting;
    
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Texture2D PreIntegratedBRDF : register(t3);
SamplerState PreIntegratedBRDFSampler : register(s3);

float3 PreintegratedSkinBxDF(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float roughness,  float opacity, float3 specularColor, float3 subsurfaceColor, float shadow)
{
    float3 lighting = DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
	
	float3 preintegratedBRDF = PreIntegratedBRDF.Sample(PreIntegratedBRDFSampler, float2(saturate(dot(normal, lightDir) * .5 + .5), 1 - opacity)).rgb;
	float3 transmission = preintegratedBRDF * subsurfaceColor;
    lighting += transmission;
    
	return lighting;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CalculateDiffuseAndSpecularColor(float specular, float metallic, inout float3 diffuseColor, inout float3 specularColor)
{
    specularColor = ComputeF0(specular, diffuseColor, metallic);
    diffuseColor = diffuseColor - diffuseColor * metallic;
}

float3 Lighting(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float roughness, float opacity, float3 specularColor, float3 emissiveColor, float3 subsurfaceColor, float metallic, float shadow, float thickness)
{        
#if SHADING_MODEL == SHADING_MODEL_UNLIT
    return Unlit(normal, lightDir, lightColor, lightIntensity, viewDir, emissiveColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_DEFAULT_LIT
    return DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_SUBSUFACE
    return SubsurfaceBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, specularColor, roughness, opacity, subsurfaceColor, shadow, thickness);
#elif SHADING_MODEL == SHADING_MODEL_PREINTEGRATED_SKIN
    return PreintegratedSkinBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, opacity, specularColor, subsurfaceColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_CLEAR_COAT
    const float clearCoat = 1.0f;
    const float clearCoatRoughness = 0.1f;
    return ClearCoatBxDF(normal, viewDir, lightDir, lightColor, lightIntensity, clearCoat, clearCoatRoughness, roughness, metallic, diffuseColor, specularColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_TWO_SIDE_FOLIAGE
    return TwoSidedBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, subsurfaceColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_HAIR
    return DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_CLOTH
    float3 fuzzColor = float3(1, 1, 1);
    float cloth = 1;
    return ClothBxDF(normal, viewDir, lightDir, lightColor, lightIntensity, fuzzColor, cloth, roughness, diffuseColor, specularColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_EYE
    return EyeBxDF(normal, viewDir, lightDir, lightColor, lightIntensity, 0, specularColor, diffuseColor, roughness, shadow);
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

float3 Lighting(LightingContext litContext, MaterialContext matContext)
{    
    float3 lighting = Lighting(litContext.Normal,
        litContext.LightDir, 
        litContext.LightColor,
        litContext.LightIntensity, 
        litContext.ViewDir,
        matContext.DiffuseColor, 
        matContext.Roughness,
        matContext.Opacity, matContext.SpecularColor,
        matContext.EmissiveColor,
        matContext.SubsurfaceColor, 
        matContext.Metallic, 
        litContext.Shadow, 
        litContext.Thickness);
        
    return  lighting;

}

float4 CaculatePointLightDirAndIntensity(float3 lightPos, float3 pixelPos, float pointLightIntensity, float invRadius)
{
    float3 toLight = lightPos - pixelPos;
    float distanceSqr = dot(toLight, toLight);
    float3 lightDir = toLight * rsqrt(distanceSqr);
    float lightIntensityPixel = saturate(1 - (distanceSqr * invRadius * invRadius)) * pointLightIntensity;

    return float4(lightDir, lightIntensityPixel);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** 
 * Compute absolute mip for a reflection capture cubemap given a roughness.
 */
float ComputeReflectionCaptureMipFromRoughness(float roughness, float cubemapMaxMip)
{
	// Heuristic that maps roughness to mip level
	// This is done in a way such that a certain mip level will always have the same roughness, regardless of how many mips are in the texture
	// Using more mips in the cubemap just allows sharper reflections to be supported
    float levelFrom1x1 = REFLECTION_CAPTURE_ROUGHEST_MIP - REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE * log2(roughness);
    return cubemapMaxMip - 1 - levelFrom1x1;
}

//---------------
// EnvBRDF
//---------------
float3 EnvBRDFApprox(float3 specularColor, float roughness, float NoV)
{
	// [ Lazarov 2013, "Getting More Physical in Call of Duty: Black Ops II" ]
	// Adaptation to fit our G term.
    const float4 c0 = { -1, -0.0275, -0.572, 0.022 };
    const float4 c1 = { 1, 0.0425, 1.04, -0.04 };
    float4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;
    float2 ab = float2(-1.04, 1.04) * a004 + r.zw;

	// Anything less than 2% is physically impossible and is instead considered to be shadowing
	// Note: this is needed for the 'specular' show flag to work, since it uses a SpecularColor of 0
    ab.y *= saturate(50.0 * specularColor.g);

    return specularColor * ab.x + ab.y;
}

TextureCube EnvironmentMap : register(t1);
SamplerState EnvironmentMapSampe : register(s2);

float3 GetImageBasedReflectionSpecular(float3 viewDir, float roughness, float3 specularColor, float3 normal)
{
    // Compute fractional mip from roughness
    float3 r = reflect(-viewDir, normal);
    float absoluteSpecularMip = ComputeReflectionCaptureMipFromRoughness(roughness, 10.0f);
    float4 specularIBLSample = EnvironmentMap.SampleLevel(EnvironmentMapSampe, r, absoluteSpecularMip);
    float3 specularIBL = RGBMDecode(specularIBLSample, 16.0f);
	specularIBL = specularIBL * specularIBL;
    return specularIBL;

}

float3 GetImageBasedReflectionLighting(LightingContext litContext, MaterialContext matContext)
{
    float3 specularLighting = GetImageBasedReflectionSpecular(litContext.ViewDir, matContext.Roughness, matContext.SpecularColor, litContext.Normal);
    float NoV = max(dot(litContext.ViewDir, litContext.Normal), 0.0);

    float3 specularColor = EnvBRDFApprox(matContext.SpecularColor, matContext.Roughness, NoV);
    float3 SpecularIBL = specularLighting * specularColor;

    return SpecularIBL;
}

float3 GetImageBasedDiffuseLighting(LightingContext litContext, MaterialContext matContext)
{
    float3 diffuseLighting = matContext.DiffuseColor.rgb * 0.1;

    return diffuseLighting;
}