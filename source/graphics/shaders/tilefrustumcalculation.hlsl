#include "tileddeferredcommon.hlsl"

// View space frustums for the grid cells.
RWStructuredBuffer<Frustum> g_PerTileFrustums : register(u0);

// A kernel to compute frustums for the grid
// This kernel is executed once per grid cell. Each thread
// computes a frustum for a grid cell.
[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void CS_ComputeFrustums(uint3 groupID           : SV_GroupID,
                        uint3 groupThreadID     : SV_GroupThreadID,
                        uint3 dispatchThreadID  : SV_DispatchThreadID,
                        uint  groupIndex        : SV_GroupIndex)
{
    // View space eye position is always at the origin.
    const float3 eyePos = float3(0.0f, 0.0f, 0.0f);

    // Compute the 4 corner points on the far clipping plane to use as the 
    // frustum vertices.
    float4 screenSpace[4];
    // Top left point
    screenSpace[0] = float4(dispatchThreadID.xy * TILE_SIZE, 1.0f, 1.0f);
    // Top right point
    screenSpace[1] = float4(float2(dispatchThreadID.x + 1, dispatchThreadID.y) * TILE_SIZE, 1.0f, 1.0f );
    // Bottom left point
    screenSpace[2] = float4(float2(dispatchThreadID.x, dispatchThreadID.y + 1 ) * TILE_SIZE, 1.0f, 1.0f );
    // Bottom right point
    screenSpace[3] = float4(float2(dispatchThreadID.x + 1, dispatchThreadID.y + 1) * TILE_SIZE, 1.0f, 1.0f );

    float3 viewSpace[4];
    // Now convert the screen space points to view space
    for (int i = 0; i < 4; i++)
    {
        viewSpace[i] = ScreenToView(screenSpace[i]).xyz;
    }

    // Now build the frustum planes from the view space points
    Frustum frustum;
 
    // Left plane
    frustum.m_Planes[0] = ComputePlane(eyePos, viewSpace[2], viewSpace[0]);
    // Right plane
	frustum.m_Planes[1] = ComputePlane(eyePos, viewSpace[1], viewSpace[3]);
    // Top plane
	frustum.m_Planes[2] = ComputePlane(eyePos, viewSpace[0], viewSpace[1]);
    // Bottom plane
	frustum.m_Planes[3] = ComputePlane(eyePos, viewSpace[3], viewSpace[2]);

    // Store the computed frustum in global memory (if our thread ID is in bounds of the grid).
    if (dispatchThreadID.x < g_NumThreads.x && dispatchThreadID.y < g_NumThreads.y)
    {
        uint index = dispatchThreadID.x + (dispatchThreadID.y * g_NumThreads.x);
        g_PerTileFrustums[index] = frustum;
    }
}