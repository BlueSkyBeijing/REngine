
// Blinn-Phong
// from: https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model
float3 BlinnPhong(float3 normal, float3 lightDir, float3 lightColor, float lightIntensity, float3 viewDir, float3 diffuseColor)
{
    const float3 ambientColor = float3(0.1f, 0.1f, 0.1f);
    const float3 specularColor = float3(1.0f, 1.0f, 1.0f);
    const float shininess = 16.0;
    
    float lambertian = max(dot(lightDir, normal), 0.0);
    float3 halfDir = normalize(lightDir + viewDir);
    float specAngle = max(dot(halfDir, normal), 0.0);
    float specular = pow(specAngle, shininess);
    
    float3 colorLinear = ambientColor +
                     diffuseColor * lambertian * lightColor * lightIntensity +
                     specularColor * specular * lightColor * lightIntensity;
    	 
    return colorLinear;
}