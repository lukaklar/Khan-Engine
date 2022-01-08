#include "clustercommon.hlsl"

cbuffer FrustumParams : register(b0)
{
    float4x4 g_Projection;
    float4x4 g_InverseProjection;
    float2   g_ScreenDimensions;
    float    g_Near;
    float    g_Far;
    float3   g_ClusterCount;
    float    g_TileSize;
}

RWStructuredBuffer<Cluster> g_Clusters : register(u0);

float4 ClipToView(float4 clip)
{
    float4 view = mul(g_InverseProjection, clip);
    
    view = view / view.w;
    
    return view;
}

float4 ScreenToView(float4 screen)
{
    float2 texCoord = screen.xy / g_ScreenDimensions;
    
    float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);
    
    return ClipToView(clip);
}

float3 LineIntersectionToZPlane(float3 A, float3 B, float zDistance)
{
    float3 normal = float3(0.0f, 0.0f, 1.0f);
    
    float3 ab = B - A;
    
    float t = (zDistance - dot(normal, A)) / dot(normal, ab);
    
    float3 result = A + t * ab;

    return result;
}

[numthreads(16, 16, 1)]
void CS_ComputeCluster(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    const float3 eyePos = float3(0.0f, 0.0f, 0.0f);

    float3 minPointVS = ScreenToView(float4(float2(dispatchThreadID.x, dispatchThreadID.y) * g_TileSize, -1.0f, 1.0f)).xyz;
    float3 maxPointVS = ScreenToView(float4(float2(dispatchThreadID.x + 1, dispatchThreadID.y + 1) * g_TileSize, -1.0f, 1.0f)).xyz;

    float tileNear = -g_Near * pow(g_Far / g_Near, dispatchThreadID.z / float(g_ClusterCount.z));
    float tileFar = -g_Near * pow(g_Far / g_Near, (dispatchThreadID.z + 1) / float(g_ClusterCount.z));
    
    float3 minPointNear = LineIntersectionToZPlane(eyePos, minPointVS, tileNear);
    float3 minPointFar = LineIntersectionToZPlane(eyePos, minPointVS, tileFar);
    float3 maxPointNear = LineIntersectionToZPlane(eyePos, maxPointVS, tileNear);
    float3 maxPointFar = LineIntersectionToZPlane(eyePos, maxPointVS, tileFar);
    
    Cluster cluster;
    cluster.m_AABBMin = min(min(minPointNear, minPointFar), min(maxPointNear, maxPointFar));
    cluster.m_AABBMax = max(max(minPointNear, minPointFar), max(maxPointNear, maxPointFar));

    if (dispatchThreadID.x < g_ClusterCount.x && dispatchThreadID.y < g_ClusterCount.y && dispatchThreadID.z < g_ClusterCount.z)
    {
        uint index = dispatchThreadID.x + dispatchThreadID.y * g_ClusterCount.x + dispatchThreadID.z * (g_ClusterCount.x + g_ClusterCount.y);
        g_Clusters[index] = cluster;
    }
}