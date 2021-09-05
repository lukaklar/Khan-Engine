#include "tileddeferredcommon.hlsl"
#include "pbrcommon.hlsl"

Texture2D g_GBuffer_Albedo : register(t0);
Texture2D g_GBuffer_Normals : register(t1);
Texture2D g_GBuffer_PBRConsts : register(t2);
Texture2D g_GBuffer_Depth : register(t3);

StructuredBuffer<Light> g_Lights : register(t4);

RWTexture2D<float4> g_LightingResult : register(u0);

struct SurfaceData
{
    float3 m_PositionVS;
	float3 m_Albedo;
	float  m_Metallic;
	float3 m_Normal;
	float  m_Roughness;
};

SurfaceData UnpackGBuffer(int3 location)
{
	SurfaceData Out;
    
    float depth = g_GBuffer_Depth.Load(location).r;
    Out.m_PositionVS = ScreenToView(float4(location.xy, depth, 1.0)).xyz;
    
    Out.m_Albedo = g_GBuffer_Albedo.Load(location).rgb;
    
	Out.m_Normal = g_GBuffer_Normals.Load(location).rgb;
	Out.m_Normal = normalize(Out.m_Normal * 2.0 - 1.0);
    
    float2 metallicAndRoughness = g_GBuffer_PBRConsts.Load(location).rg;
	Out.m_Metallic = metallicAndRoughness.r;
	Out.m_Roughness = metallicAndRoughness.g;

	return Out;
}

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void CS_DeferredLighting(uint3 groupID           : SV_GroupID,
                         uint3 groupThreadID     : SV_GroupThreadID,
                         uint3 dispatchThreadID  : SV_DispatchThreadID,
                         uint  groupIndex        : SV_GroupIndex)
{
    int3 texCoord = int3(dispatchThreadID.xy, 0);
    SurfaceData data = UnpackGBuffer(texCoord);
    
    //float3 eyePos = float3(0, 0, 0);
    float3 V = normalize(float3(0, 0, 0) - data.m_PositionVS);
    
    // Specular contribution
    float3 Lo = float3(0.0, 0.0, 0.0);
    for (uint i = 0; i < g_NumLights; i++)
    {
        Light light = g_Lights[i];
        
        float3 L;
        float3 radiance;
        switch (light.m_Type)
        {
            case DIRECTIONAL_LIGHT:
            {
                L = normalize(-light.m_DirectionVS);
                radiance = light.m_Color * light.m_Luminance;
                break;
            }
            case OMNI_LIGHT:
            {
                L = light.m_PositionVS - data.m_PositionVS;
                float distance = length(L);
                L /= distance;
                float attenuation = 1.0 / (distance * distance);
                radiance = light.m_Color * attenuation * light.m_Luminance;
                break;
            }
            case SPOT_LIGHT:
            {
                L = light.m_PositionVS - data.m_PositionVS;
                float distance = length(L);
                L /= distance;
                float attenuation = 1.0 / (distance * distance);
                radiance = light.m_Color * attenuation * light.m_Luminance;
                // check the angle between L and lightDirection and calculate falloff
                break;
            }
        }
        
        Lo += BRDF(L, V, data.m_Normal, data.m_Metallic, data.m_Roughness, data.m_Albedo, radiance);
    }

	// Combine with ambient
    float3 color = data.m_Albedo * 0.02 + Lo;
    color = pow(color, float3(0.4545, 0.4545, 0.4545));
    
    g_LightingResult[texCoord.xy] = float4(color, 1.0);
}