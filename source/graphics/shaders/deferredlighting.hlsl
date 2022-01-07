#include "pbrcommon.hlsl"

#define TILE_SIZE 16

#define DIRECTIONAL_LIGHT 0
#define OMNI_LIGHT 1
#define SPOT_LIGHT 2

struct Light
{
	uint   m_Type;
	float3 m_PositionVS;
	float3 m_DirectionVS;
    float  m_Range;
	float3 m_Color;
	float  m_Luminance;
	float  m_SpotlightAngle;
    float3 m_Padding;
};

cbuffer FrustumParams : register(b0)
{
    float4x4 g_Projection;
    float4x4 g_InverseProjection;
    float2   g_ScreenDimensions;
    float    g_Near;
    float    g_Far;
    float3   g_ClusterCount;
}

cbuffer LightParams : register(b1)
{
    uint g_NumLights;
}

// Convert clip space coordinates to view space
float4 ClipToView(float4 clip)
{
    // View space position.
    float4 view = mul(g_InverseProjection, clip);
    // Perspective projection.
    view = view / view.w;
 
    return view;
}

// Convert screen space coordinates to view space.
float4 ScreenToView(float4 screen)
{
    // Convert to normalized texture coordinates
    float2 texCoord = screen.xy / g_ScreenDimensions;
 
    // Convert to clip space
    float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);
 
    return ClipToView(clip);
}

Texture2D g_GBuffer_Albedo : register(t0);
Texture2D g_GBuffer_Normals : register(t1);
Texture2D g_GBuffer_PBRConsts : register(t2);
Texture2D g_GBuffer_Depth : register(t3);
Texture2D g_AOTexture : register(t4);
StructuredBuffer<Light> g_Lights : register(t5);

RWTexture2D<float4> g_LightingResult : register(u0);

struct SurfaceData
{
    float3 m_PositionVS;
	float4 m_Albedo;
	float  m_Metallic;
	float3 m_Normal;
	float  m_Roughness;
};

SurfaceData UnpackGBuffer(int3 location)
{
	SurfaceData Out;
    
    float depth = g_GBuffer_Depth.Load(location).r;
    Out.m_PositionVS = ScreenToView(float4(location.xy, depth, 1.0)).xyz;
    
    Out.m_Albedo = g_GBuffer_Albedo.Load(location);
    
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
        
        Lo += BRDF(L, V, data.m_Normal, data.m_Metallic, data.m_Roughness, data.m_Albedo.rgb, radiance);
    }

	// Combine with ambient
    float3 color = data.m_Albedo.rgb * 0.02 * g_AOTexture[dispatchThreadID.xy].r + Lo;
    if (data.m_Albedo.a == 0.0f)
    {
        color = float3(0.5f, 0.5f, 0.5f);
    }
    
    g_LightingResult[texCoord.xy] = float4(color, 1.0);
}