#include "clustercommon.hlsl"

cbuffer FrustumParams : register(b0, space0)
{
    float4x4 g_Projection;
    float4x4 g_InverseProjection;
    float2   g_ScreenDimensions;
    float    g_Near;
    float    g_Far;
    float3   g_ClusterCount;
    float    g_TileSize;
}

cbuffer PerFrame : register(b1, space0)
{
    float4x4 g_ViewProjection;
};

cbuffer PerObject : register(b0, space2)
{
    float4x4 g_ModelMatrix;
};

RWBuffer<uint> g_ActiveClusterMask : register(u0, space0);

uint GetDepthSlice(float z)
{
    return floor(log(z) * g_ClusterCount.z / log(g_Near / g_Far) - g_ClusterCount.z * log(g_Near) / log(g_Far / g_Near));
}

float4 VS_MarkActiveClusters(float3 position : POSITION) : SV_Position
{
    return mul(g_ViewProjection, mul(g_ModelMatrix, float4(position, 1.0)));
}

void PS_MarkActiveClusters(float4 position : SV_Position)
{
    uint slice = GetDepthSlice(position.z);
    uint3 clusterID = uint3(position.xy / g_TileSize, slice);

    uint index = clusterID.x + clusterID.y * g_ClusterCount.x + clusterID.z * (g_ClusterCount.x + g_ClusterCount.y);
    g_ActiveClusterMask[index] = true;
}

cbuffer ClusterParams : register(b0)
{
    uint g_NumClusters;
}

Buffer<uint> g_ActiveClusterFlags : register(t0);

RWBuffer<uint> g_ActiveClusterIndexList : register(u0);
RWBuffer<uint> g_CullingDispatchArgs : register(u1);

[numthreads(64, 1, 1)]
void CS_CompactActiveClusters(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x == 0)
    {
        g_CullingDispatchArgs[0] = 0;
        g_CullingDispatchArgs[1] = 1;
        g_CullingDispatchArgs[2] = 1;
    }
    
    DeviceMemoryBarrierWithGroupSync();
    
    uint clusterIndex = dispatchThreadID.x;
    if (clusterIndex < g_NumClusters && g_ActiveClusterFlags[clusterIndex])
    {
        uint index;
        InterlockedAdd(g_CullingDispatchArgs[0], 1, index);
        g_ActiveClusterIndexList[index] = clusterIndex;
    }
}

// ======================== Alternate version ========================

//groupshared uint gs_ActiveClusterIndexCounter;
//groupshared uint gs_ActiveClusterIndexStartOffset;
//groupshared uint gs_ActiveClusterIndexList[64];

//[numthreads(64, 1, 1)]
//void CS_CompactActiveClusters2(uint3 dispatchThreadID : SV_DispatchThreadID, uint groupIndex : SV_GroupIndex)
//{
//    if (dispatchThreadID.x == 0)
//    {
//        g_ActiveClusterIndexCounter[0] = 0;
//    }
    
//    if (groupIndex == 0)
//    {
//        gs_ActiveClusterIndexCounter = 0;
//    }
    
//    GroupMemoryBarrierWithGroupSync();
    
//    uint clusterIndex = dispatchThreadID.x;
//    if (clusterIndex < g_NumClusters && g_ActiveClusterFlags[clusterIndex])
//    {
//        uint index;
//        InterlockedAdd(gs_ActiveClusterIndexCounter, 1, index);
//        gs_ActiveClusterIndexList[index] = clusterIndex;
//    }
    
//    GroupMemoryBarrierWithGroupSync();
    
//    if (groupIndex == 0)
//    {
//        InterlockedAdd(g_ActiveClusterIndexCounter[0], gs_ActiveClusterIndexCounter, gs_ActiveClusterIndexStartOffset);   
//    }
    
//    GroupMemoryBarrierWithGroupSync();
    
//    for (uint i = groupIndex; i < gs_ActiveClusterIndexCounter; i += 64)
//    {
//        g_ActiveClusterIndexList[gs_ActiveClusterIndexStartOffset + i] = gs_ActiveClusterIndexList[i];
//    }
//}