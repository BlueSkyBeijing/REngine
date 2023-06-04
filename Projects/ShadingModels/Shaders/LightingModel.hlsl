#include <Common.hlsl>

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
	// OpacityScale of 0 gives no normal dependent lighting, OpacityScale of 1 gives strong normal contribution
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

///////////////////////////////////////////////////////////////////////////////////////////////////
// Transmittance functions

struct FHairTransmittanceData
{
    bool bUseLegacyAbsorption;
    bool bUseSeparableR;
    bool bUseBacklit;

    float  OpaqueVisibility;
    float3 LocalScattering;
    float3 GlobalScattering;

};

FHairTransmittanceData InitHairTransmittanceData(bool bMultipleScatterEnable = true)
{
    FHairTransmittanceData o;
    o.bUseLegacyAbsorption = true;
    o.bUseSeparableR = true;
    o.bUseBacklit = false;

    o.OpaqueVisibility = 1;
    o.LocalScattering = 0;
    o.GlobalScattering = 1;

    return o;
}

FHairTransmittanceData InitHairStrandsTransmittanceData(bool bMultipleScatterEnable = false)
{
    FHairTransmittanceData o = InitHairTransmittanceData(bMultipleScatterEnable);
    o.bUseLegacyAbsorption = false;
    o.bUseBacklit = true;
    return o;
}

// Utility functions
float Hair_g(float B, float Theta)
{
    return exp(-0.5 * Pow2(Theta) / (B * B)) / (sqrt(2 * PI) * B);
}

float Hair_F(float CosTheta)
{
    const float n = 1.55;
    const float F0 = Pow2((1 - n) / (1 + n));
    return F0 + (1 - F0) * Pow5(1 - CosTheta);
}

float3 KajiyaKayDiffuseAttenuation(float Metallic, float3 BaseColor, float3 L, float3 V, half3 N, float Shadow)
{
    // Use soft Kajiya Kay diffuse attenuation
    float KajiyaDiffuse = 1 - abs(dot(N, L));

    float3 FakeNormal = normalize(V - N * dot(V, N));
    //N = normalize( DiffuseN + FakeNormal * 2 );
    N = FakeNormal;

    // Hack approximation for multiple scattering.
    float Wrap = 1;
    float NoL = saturate((dot(N, L) + Wrap) / Square(1 + Wrap));
    float DiffuseScatter = (1 / PI) * lerp(NoL, KajiyaDiffuse, 0.33) * Metallic;
    float Luma = Luminance(BaseColor);
    float3 ScatterTint = pow(abs(BaseColor / Luma), 1 - Shadow);
    return sqrt(abs(BaseColor)) * DiffuseScatter * ScatterTint;
}

float3 EvaluateHairMultipleScattering(
    const FHairTransmittanceData TransmittanceData,
    const float Roughness,
    const float3 Fs)
{
    return TransmittanceData.GlobalScattering * (Fs + TransmittanceData.LocalScattering) * TransmittanceData.OpaqueVisibility;
}

float3 noise(float3 dir, float3 pos, float intensity)
{
    float3 p0 = floor(pos);
    float3 p1 = p0 + float3(1.0, 0.0, 0.0);
    float3 p2 = p0 + float3(0.0, 1.0, 0.0);
    float3 p3 = p0 + float3(1.0, 1.0, 0.0);
    float3 p4 = p0 + float3(0.0, 0.0, 1.0);
    float3 p5 = p0 + float3(1.0, 0.0, 1.0);
    float3 p6 = p0 + float3(0.0, 1.0, 1.0);
    float3 p7 = p0 + float3(1.0, 1.0, 1.0);

    float3 d0 = dir * (pos - p0);
    float3 d1 = dir * (pos - p1);
    float3 d2 = dir * (pos - p2);
    float3 d3 = dir * (pos - p3);
    float3 d4 = dir * (pos - p4);
    float3 d5 = dir * (pos - p5);
    float3 d6 = dir * (pos - p6);
    float3 d7 = dir * (pos - p7);

    float3 g0 = normalize(d0);
    float3 g1 = normalize(d1);
    float3 g2 = normalize(d2);
    float3 g3 = normalize(d3);
    float3 g4 = normalize(d4);
    float3 g5 = normalize(d5);
    float3 g6 = normalize(d6);
    float3 g7 = normalize(d7);

    float n0 = dot(g0, d0);
    float n1 = dot(g1, d1);
    float n2 = dot(g2, d2);
    float n3 = dot(g3, d3);
    float n4 = dot(g4, d4);
    float n5 = dot(g5, d5);
    float n6 = dot(g6, d6);
    float n7 = dot(g7, d7);

    float3 weights = smoothstep(0.0, 1.0, abs(dir));
    float4x4 b = float4x4(
        n0, n1, n2, n3,
        n4, n5, n6, n7,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0
        );

    float3 offset = intensity * normalize(dot(weights, mul(b, float4(1.0, pos.x - p0.x, pos.y - p0.y, pos.z - p0.z))));
    float3 newDir = dir + offset;

    return normalize(newDir);
}

// Hair BSDF
// Approximation to HairShadingRef using concepts from the following papers:
// [Marschner et al. 2003, "Light Scattering from Human Hair Fibers"]
// [Pekelis et al. 2015, "A Data-Driven Light Scattering Model for Hair"]
float3 HairShading(float3 BaseColor, float Metallic, float Roughness, float Specular, float3 L, float3 V, half3 N, float Shadow, FHairTransmittanceData HairTransmittance, float InBacklit, float Area, uint2 Random)
{
    // to prevent NaN with decals
    // OR-18489 HERO: IGGY: RMB on E ability causes blinding hair effect
    // OR-17578 HERO: HAMMER: E causes blinding light on heroes with hair
    float ClampedRoughness = clamp(Roughness, 1 / 255.0f, 1.0f);

    //const float3 DiffuseN	= OctahedronToUnitVector( GBuffer.CustomData.xy * 2 - 1 );
    const float Backlit = min(InBacklit, 1);

    // UE-155845: KajiyaKayDiffuseAttenuation() is not energy conservative, so we define a correction factor to prevent it from adding light.
    // This is not actually physically correct and simply a hack to prevent certain objects with hair materials from glowing when Lumen is enabled.
    float KajiyaKayDiffuseFactor = 1.0f;

    // N is the vector parallel to hair pointing toward root
    //N = noise(N, V*100, 0.5);
    const float VoL = dot(V, L);
    const float SinThetaL = clamp(dot(N, L), -1.f, 1.f);
    const float SinThetaV = clamp(dot(N, V), -1.f, 1.f);
    float CosThetaD = cos(0.5 * abs(asin(SinThetaV) - asin(SinThetaL)));

    //CosThetaD = abs( CosThetaD ) < 0.01 ? 0.01 : CosThetaD;

    const float3 Lp = L - SinThetaL * N;
    const float3 Vp = V - SinThetaV * N;
    const float CosPhi = dot(Lp, Vp) * rsqrt(dot(Lp, Lp) * dot(Vp, Vp) + 1e-4);
    const float CosHalfPhi = sqrt(saturate(0.5 + 0.5 * CosPhi));
    //const float Phi = acosFast( CosPhi );

    float n = 1.55;
    //float n_prime = sqrt( n*n - 1 + Pow2( CosThetaD ) ) / CosThetaD;
    float n_prime = 1.19 / CosThetaD + 0.36 * CosThetaD;

    float Shift = 0.035;
    float Alpha[] =
    {
        -Shift * 2,
        Shift,
        Shift * 4,
    };
    float B[] =
    {
        Area + Pow2(ClampedRoughness),
        Area + Pow2(ClampedRoughness) / 2,
        Area + Pow2(ClampedRoughness) * 2,
    };

    float3 S = 0;
    {
        const float sa = sin(Alpha[0]);
        const float ca = cos(Alpha[0]);
        float ShiftR = 2 * sa * (ca * CosHalfPhi * sqrt(1 - SinThetaV * SinThetaV) + sa * SinThetaV);
        float BScale = HairTransmittance.bUseSeparableR ? sqrt(2.0) * CosHalfPhi : 1;
        float Mp = Hair_g(B[0] * BScale, SinThetaL + SinThetaV - ShiftR);
        float Np = 0.25 * CosHalfPhi;
        float Fp = Hair_F(sqrt(saturate(0.5 + 0.5 * VoL)));
        S += Mp * Np * Fp * (Specular * 2) * lerp(1, Backlit, saturate(-VoL));

        KajiyaKayDiffuseFactor -= Fp;
    }

    // TT
    {
        float Mp = Hair_g(B[1], SinThetaL + SinThetaV - Alpha[1]);

        float a = 1 / n_prime;
        //float h = CosHalfPhi * rsqrt( 1 + a*a - 2*a * sqrt( 0.5 - 0.5 * CosPhi ) );
        //float h = CosHalfPhi * ( ( 1 - Pow2( CosHalfPhi ) ) * a + 1 );
        float h = CosHalfPhi * (1 + a * (0.6 - 0.8 * CosPhi));
        //float h = 0.4;
        //float yi = asinFast(h);
        //float yt = asinFast(h / n_prime);

        float f = Hair_F(CosThetaD * sqrt(saturate(1 - h * h)));
        float Fp = Pow2(1 - f);
        //float3 Tp = pow( GBuffer.BaseColor, 0.5 * ( 1 + cos(2*yt) ) / CosThetaD );
        //float3 Tp = pow( GBuffer.BaseColor, 0.5 * cos(yt) / CosThetaD );
        float3 Tp = 0;
        {
            Tp = pow(abs(BaseColor), 0.5 * sqrt(1 - Pow2(h * a)) / CosThetaD);
        }

        //float t = asin( 1 / n_prime );
        //float d = ( sqrt(2) - t ) / ( 1 - t );
        //float s = -0.5 * PI * (1 - 1 / n_prime) * log( 2*d - 1 - 2 * sqrt( d * (d - 1) ) );
        //float s = 0.35;
        //float Np = exp( (Phi - PI) / s ) / ( s * Pow2( 1 + exp( (Phi - PI) / s ) ) );
        //float Np = 0.71 * exp( -1.65 * Pow2(Phi - PI) );
        float Np = exp(-3.65 * CosPhi - 3.98);

        S += Mp * Np * Fp * Tp * Backlit;

        KajiyaKayDiffuseFactor -= Fp;
    }

    // TRT
    {
        float Mp = Hair_g(B[2], SinThetaL + SinThetaV - Alpha[2]);

        //float h = 0.75;
        float f = Hair_F(CosThetaD * 0.5);
        float Fp = Pow2(1 - f) * f;
        //float3 Tp = pow( GBuffer.BaseColor, 1.6 / CosThetaD );
        float3 Tp = pow(abs(BaseColor), 0.8 / CosThetaD);

        //float s = 0.15;
        //float Np = 0.75 * exp( Phi / s ) / ( s * Pow2( 1 + exp( Phi / s ) ) );
        float Np = exp(17 * CosPhi - 16.78);

        S += Mp * Np * Fp * Tp;

        KajiyaKayDiffuseFactor -= Fp;
    }

    {
        S = EvaluateHairMultipleScattering(HairTransmittance, ClampedRoughness, S);
        S += KajiyaKayDiffuseAttenuation(Metallic, BaseColor, L, V, N, Shadow) * saturate(KajiyaKayDiffuseFactor);
    }

    S = -min(-S, 0.0);
    return S;
}

float3 HairBxDF(float3 lightColor, float lightIntensity, half3 N, half3 V, half3 L, float shadow, float thickness, float4 baseColor, float metallic, float roughness, float specular)
{
    float TransmissionShadow = thickness;
    FHairTransmittanceData HairTransmittance = InitHairTransmittanceData();
    const float3 BsdfValue = HairShading(baseColor.rgb, metallic, roughness, specular, L, V, N, TransmissionShadow, HairTransmittance, 1, 0, uint2(0, 0));

    float3 transmission = BsdfValue * shadow;

    float3 lighting = transmission;
    lighting *= lightColor * lightIntensity;

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

float3 EyeBxDF(float3 normal, float3 viewDir, float3 lightDir, float3 lightColor, float lightIntensity, float specular, float3 specularColor, float3 diffuseColor, float4 baseColor, float roughness, float shadow )
{
    float NoL = max(dot(lightDir, normal), 0.00001);

    float2 oct = 1;
    float3 irisNormal		= OctahedronToUnitVector(oct);
	const float  irisDistance	= 1;
	const float  irisMask		= baseColor.a;

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

float3 PreintegratedSkinBxDF(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float roughness,  float opacity, float3 specularColor, float3 subsurfaceColor, float shadow, float thickness)
{
    float3 lighting = DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
	
	float3 preintegratedBRDF = PreIntegratedBRDF.Sample(PreIntegratedBRDFSampler, float2(saturate(dot(normal, lightDir) * .5 + .5), 1 - opacity)).rgb;
	float3 transmission = preintegratedBRDF * subsurfaceColor;
    lighting += transmission * thickness;
    
	return lighting;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CalculateDiffuseAndSpecularColor(float specular, float metallic, inout float3 diffuseColor, inout float3 specularColor)
{
    specularColor = ComputeF0(specular, diffuseColor, metallic);
    diffuseColor = diffuseColor - diffuseColor * metallic;
}

float3 Lighting(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor, float roughness, float opacity, float3 specularColor, float3 emissiveColor, float3 subsurfaceColor, float metallic, float shadow, float thickness, float specular, float4 baseColor)
{        
#if SHADING_MODEL == SHADING_MODEL_UNLIT
    return Unlit(normal, lightDir, lightColor, lightIntensity, viewDir, emissiveColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_DEFAULT_LIT
    return DefaultLitBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_SUBSUFACE
    return SubsurfaceBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, specularColor, roughness, opacity, subsurfaceColor, shadow, thickness);
#elif SHADING_MODEL == SHADING_MODEL_PREINTEGRATED_SKIN
    return PreintegratedSkinBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, opacity, specularColor, subsurfaceColor, shadow, thickness);
#elif SHADING_MODEL == SHADING_MODEL_CLEAR_COAT
    const float clearCoat = 1.0f;
    const float clearCoatRoughness = 0.1f;
    return ClearCoatBxDF(normal, viewDir, lightDir, lightColor, lightIntensity, clearCoat, clearCoatRoughness, roughness, metallic, diffuseColor, specularColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_TWO_SIDE_FOLIAGE
    return TwoSidedBxDF(normal, lightDir, lightColor, lightIntensity, viewDir, diffuseColor, roughness, specularColor, subsurfaceColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_HAIR
    return HairBxDF(lightColor, lightIntensity, normal, lightDir, viewDir, shadow, thickness, baseColor, metallic, roughness, specular);
#elif SHADING_MODEL == SHADING_MODEL_CLOTH
    float3 fuzzColor = subsurfaceColor;
    float cloth = 1;
    return ClothBxDF(normal, viewDir, lightDir, lightColor, lightIntensity, fuzzColor, cloth, roughness, diffuseColor, specularColor, shadow);
#elif SHADING_MODEL == SHADING_MODEL_EYE
    return EyeBxDF(normal, viewDir, lightDir, lightColor, lightIntensity, 0, specularColor, diffuseColor, baseColor, roughness, shadow);
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
        matContext.Opacity, 
        matContext.SpecularColor,
        matContext.EmissiveColor,
        matContext.SubsurfaceColor, 
        matContext.Metallic, 
        litContext.Shadow, 
        litContext.Thickness,
        matContext.Specular,
        matContext.BaseColor);
        
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
float2 EnvBRDFApproxLazarov(float roughness, float NoV)
{
    // [ Lazarov 2013, "Getting More Physical in Call of Duty: Black Ops II" ]
    // Adaptation to fit our G term.
    const float4 c0 = { -1, -0.0275, -0.572, 0.022 };
    const float4 c1 = { 1, 0.0425, 1.04, -0.04 };
    float4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;
    float2 ab = float2(-1.04, 1.04) * a004 + r.zw;
    return ab;
}

float3 EnvBRDFApprox(float3 specularColor, float roughness, float NoV)
{
    float2 ab = EnvBRDFApproxLazarov(roughness, NoV);

    // Anything less than 2% is physically impossible and is instead considered to be shadowing
    // Note: this is needed for the 'specular' show flag to work, since it uses a SpecularColor of 0
    float f90 = saturate(50.0 * specularColor.g);

    return specularColor * ab.x + f90 * ab.y;
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
    float3 specularIBL = specularLighting * specularColor;

    return specularIBL;
}

float3 GetImageBasedDiffuseLighting(LightingContext litContext, MaterialContext matContext)
{
    float3 diffuseLighting = matContext.DiffuseColor.rgb * 0.1;

    return diffuseLighting;
}

void RemapClearCoatDiffuseAndSpecularColor(float3 baseColor, float roughness, float metallic, float specular, float clearCoat, float NoV, inout float3 diffuseColor, inout float3 specularColor)
{
    // Attenuate base color and recompute diffuse color
    float refractionScale = ((NoV * 0.5f + 0.5f) * NoV - 1) * saturate(1.25f - 1.25f * roughness) + 1;

    float metalSpec = 0.9f;
    float3 absorptionColor = baseColor * (1 / metalSpec);
    float3 absorption = absorptionColor * ((NoV - 1) * 0.85 * (1 - lerp(absorptionColor, Square(absorptionColor), -0.78f)) + 1);

    float f0 = 0.04f;
    float fc = Pow5(1 - NoV);
    float f = fc + (1 - fc) * f0;
    float layerAttenuation = lerp(1, (1 - f), clearCoat);

    float3 baseColorCoat = lerp(baseColor * layerAttenuation, metalSpec * absorption * refractionScale, metallic * clearCoat);
    //BaseColor += Dither / 255.f;
    diffuseColor = baseColorCoat - baseColorCoat * metallic;

    float ccSpecular = lerp(specular, refractionScale, clearCoat);
    specularColor = ComputeF0(ccSpecular, baseColorCoat, metallic);
}
