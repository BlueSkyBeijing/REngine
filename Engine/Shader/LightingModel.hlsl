
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