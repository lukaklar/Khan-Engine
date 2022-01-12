#include "clustercommon.hlsl"

struct Sphere
{
    float3 m_Center;
    float  m_Radius;
};

struct Cone
{
    float3 m_Origin;
    float  m_Angle;
    float3 m_Forward;
    float  m_Size;
};

cbuffer LightListParams : register(b0)
{
    uint g_NumLights;
}

StructuredBuffer<Cluster> g_Clusters : register(t0);
Buffer<uint> g_ActiveClusterIndexList : register(t1);
StructuredBuffer<Light> g_Lights : register(t2);

RWStructuredBuffer<uint> g_LightIndexCounter : register(u0);
RWBuffer<uint> g_LightIndexList : register(u1);
RWBuffer<uint2> g_LightGrid : register(u2);

groupshared Cluster gs_GroupCluster;
groupshared uint gs_GroupClusterIndex;

groupshared uint gs_LightCount;
groupshared uint gs_LightIndexStartOffset;
groupshared uint gs_LightList[1024];

void AppendLight(uint lightIndex)
{
    uint index;
	InterlockedAdd(gs_LightCount, 1, index);
    if (index < 1024)
    {
		gs_LightList[index] = lightIndex;
	}
}

bool SphereIntersectsCluster(Sphere sphere, Cluster cluster)
{
    float3 closestPoint = max(cluster.m_AABBMin, min(sphere.m_Center, cluster.m_AABBMax));
    
    return length(closestPoint - sphere.m_Center) < sphere.m_Radius;
}

bool ConeIntersectsCluster(Cone cone, Cluster cluster)
{
    float3 V = cluster.m_SphereCenter - cone.m_Origin;
    float VlenSq = dot(V, V);
    float V1len = dot(V, cone.m_Forward);
    float distanceClosestPoint = cos(cone.m_Angle) * sqrt(VlenSq - V1len * V1len) - V1len * sin(cone.m_Angle);
 
    bool angleCull = distanceClosestPoint > cluster.m_SphereRadius;
    bool frontCull = V1len > cluster.m_SphereRadius + cone.m_Size;
    bool backCull = V1len < -cluster.m_SphereRadius;
    
    return !(angleCull || frontCull || backCull);
}

[numthreads(128, 1, 1)]
void CS_CullLights(uint3 dispatchThreadID : SV_DispatchThreadID, uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    if (dispatchThreadID.x == 0)
    {
        g_LightIndexCounter[0] = 0;
    }
    
    DeviceMemoryBarrierWithGroupSync();
    
    if (groupIndex == 0)
    {
        gs_LightCount = 0;
        gs_GroupClusterIndex = g_ActiveClusterIndexList[groupID.x];
        gs_GroupCluster = g_Clusters[gs_GroupClusterIndex];
    }

    GroupMemoryBarrierWithGroupSync();

    for (uint i = groupIndex; i < g_NumLights; i += 128)
    {
        Light light = g_Lights[i];

        switch (light.m_Type)
        {
            case OMNI_LIGHT:
            {
                Sphere sphere = { light.m_PositionVS, light.m_Range };
                if (SphereIntersectsCluster(sphere, gs_GroupCluster))
                {
                    AppendLight(i);
                }
                break;
            }
            case SPOT_LIGHT:
            {
                Cone cone = { light.m_PositionVS, light.m_OuterConeAngle, light.m_DirectionVS, light.m_Range };
                if (ConeIntersectsCluster(cone, gs_GroupCluster))
                {
                    AppendLight(i);
                }
                break;
            }
            case DIRECTIONAL_LIGHT:
            {
                AppendLight(i);
                break;
            }
        }
    }
 
    GroupMemoryBarrierWithGroupSync();

    if (groupIndex == 0)
    {
        InterlockedAdd(g_LightIndexCounter[0], gs_LightCount, gs_LightIndexStartOffset);
        g_LightGrid[gs_GroupClusterIndex] = uint2(gs_LightIndexStartOffset, gs_LightCount);
    }
 
    GroupMemoryBarrierWithGroupSync();

	for (i = groupIndex; i < gs_LightCount; i += 128)
    {
		g_LightIndexList[gs_LightIndexStartOffset + i] = gs_LightList[i];
	}
}