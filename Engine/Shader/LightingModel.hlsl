
// Blinn-Phong

float3 BlinnPhong(float3 normal, float3 lightDir, float3 lightColor, float3 specularColor, float specPower, float3 viewDir)
{
    float NdotL = max(0, dot(normal, lightDir));
	 
    float3 halfVector = normalize(lightDir + viewDir);
    float NdotH = max(0, dot(normal, halfVector));
    float3 spec = pow(NdotH, specPower) * specularColor;
	 
    return lightColor.rgb * NdotL + lightColor.rgb * specularColor.rgb * spec;
}