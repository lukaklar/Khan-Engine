static const float PI = 3.14159265359;

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2) / (PI * denom * denom);
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float GL = dotNL / (dotNL * (1.0 - k) + k);
    float GV = dotNV / (dotNV * (1.0 - k) + k);
    return GL * GV;
}

// Fresnel function ----------------------------------------------------
float3 F_Schlick(float cosTheta, float metallic, float3 albedo, float3 specularReflectance)
{
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic) * specularReflectance;
    float3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    return F;
}

// Specular BRDF composition --------------------------------------------
float3 BRDF(float3 L, float3 V, float3 N, float metallic, float roughness, float3 albedo, float3 specularReflectance, float3 radiance)
{
	// Precalculate vectors and dot products
    float3 H = normalize(V + L);
    float  dotNV = clamp(dot(N, V), 0.0, 1.0);
    float  dotNL = clamp(dot(N, L), 0.0, 1.0);
    float  dotLH = clamp(dot(L, H), 0.0, 1.0);
    float  dotNH = clamp(dot(N, H), 0.0, 1.0);

    float3 color = float3(0.0, 0.0, 0.0);

    if (dotNL > 0.0)
    {
        float rroughness = max(0.05, roughness);
		// D = Normal distribution (Distribution of the microfacets)
        float D = D_GGX(dotNH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
        float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
        float3 F = F_Schlick(dotNV, metallic, albedo, specularReflectance);

        float3 spec = D * F * G / (4.0 * dotNL * dotNV);
        
        float3 diff = (float3(1.0, 1.0, 1.0) - F) * (1.0 - metallic) * albedo / PI;

        color += (diff + spec) * dotNL * radiance;
    }

    return color;
}