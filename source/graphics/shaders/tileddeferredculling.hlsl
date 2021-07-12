#include "tileddeferredcommon.hlsl"

struct Sphere
{
    float3 m_Center;
    float  m_Radius;
};

struct Cone
{
    float3 m_Tip;       // Cone tip.
    float  m_Height;    // Height of the cone.
    float3 m_Direction; // Direction of the cone.
    float  m_Radius;    // Bottom radius of the cone.
};

// Check to see if a sphere is fully behind (inside the negative halfspace of) a plane.
// Source: Real-time collision detection, Christer Ericson (2005)
bool SphereInsidePlane(Sphere sphere, Plane plane)
{
    return dot(plane.m_Normal, sphere.m_Center) - plane.m_Distance < -sphere.m_Radius;
}

// Check to see of a light is partially contained within the frustum.
bool SphereInsideFrustum(Sphere sphere, Frustum frustum, float zNear, float zFar)
{
    bool result = true;
 
    // First check depth
    // Note: Here, the view vector points in the -Z axis so the 
    // far depth value will be approaching -infinity.
    if (sphere.m_Center.z - sphere.m_Radius > zFar || sphere.m_Center.z + sphere.m_Radius < zNear)
    {
        result = false;
    }
 
    // Then check frustum planes
    for (int i = 0; i < 4 && result; i++)
    {
        if (SphereInsidePlane(sphere, frustum.m_Planes[i]))
        {
            result = false;
        }
    }
 
    return result;
}

// Check to see if a point is fully behind (inside the negative halfspace of) a plane.
bool PointInsidePlane(float3 p, Plane plane)
{
    return dot(plane.m_Normal, p) - plane.m_Distance < 0;
}

// Check to see if a cone if fully behind (inside the negative halfspace of) a plane.
// Source: Real-time collision detection, Christer Ericson (2005)
bool ConeInsidePlane(Cone cone, Plane plane)
{
    // Compute the farthest point on the end of the cone to the positive space of the plane.
    float3 m = cross(cross(plane.m_Normal, cone.m_Direction), cone.m_Direction);
    float3 Q = cone.m_Tip + cone.m_Direction * cone.m_Height - m * cone.m_Radius;
 
    // The cone is in the negative halfspace of the plane if both
    // the tip of the cone and the farthest point on the end of the cone to the 
    // positive halfspace of the plane are both inside the negative halfspace 
    // of the plane.
    return PointInsidePlane(cone.m_Tip, plane) && PointInsidePlane(Q, plane);
}

bool ConeInsideFrustum(Cone cone, Frustum frustum, float zNear, float zFar)
{
    bool result = true;
 
    Plane nearPlane = { float3(0, 0, -1), -zNear };
    Plane farPlane = { float3(0, 0, 1), zFar };
 
    // First check the near and far clipping planes.
    if (ConeInsidePlane(cone, nearPlane) || ConeInsidePlane(cone, farPlane))
    {
        result = false;
    }
 
    // Then check frustum planes
    for (int i = 0; i < 4 && result; i++)
    {
        if (ConeInsidePlane(cone, frustum.m_Planes[i]))
        {
            result = false;
        }
    }
 
    return result;
}

// The depth from the screen space texture.
Texture2D g_DepthTexture : register(t0);
// Precomputed frustums for the grid.
StructuredBuffer<Frustum> g_PerTileFrustums : register(t1);
// List of scene lights
StructuredBuffer<Light> g_Lights : register(t2);

// Global counter for current index into the light index list.
RWStructuredBuffer<uint> g_OpaqueLightIndexCounter : register(u0);
RWStructuredBuffer<uint> g_TransparentLightIndexCounter : register(u1);

// Light index lists and light grids.
RWStructuredBuffer<uint> g_OpaqueLightIndexList : register(u2);
RWStructuredBuffer<uint> g_TransparentLightIndexList : register(u3);
RWTexture2D<uint2> g_OpaqueLightGrid : register(u4);
RWTexture2D<uint2> g_TransparentLightGrid : register(u5);

groupshared uint gs_uMinDepth;
groupshared uint gs_uMaxDepth;

groupshared Frustum gs_GroupFrustum;

// Opaque geometry light lists.
groupshared uint gs_OpaqueLightCount;
groupshared uint gs_OpaqueLightIndexStartOffset;
groupshared uint gs_OpaqueLightList[1024];
 
// Transparent geometry light lists.
groupshared uint gs_TransparentLightCount;
groupshared uint gs_TransparentLightIndexStartOffset;
groupshared uint gs_TransparentLightList[1024];

// Add the light to the visible light list for opaque geometry.
void AppendLightForOpaqueGeometry(uint lightIndex)
{
    uint index; // Index into the visible lights array.
	InterlockedAdd(gs_OpaqueLightCount, 1, index);
    if (index < 1024)
    {
		gs_OpaqueLightList[index] = lightIndex;
	}
}
 
// Add the light to the visible light list for transparent geometry.
void AppendLightForTransparentGeometry(uint lightIndex)
{
    uint index; // Index into the visible lights array.
	InterlockedAdd(gs_TransparentLightCount, 1, index);
    if (index < 1024)
    {
		gs_TransparentLightList[index] = lightIndex;
	}
}

// Implementation of light culling compute shader is based on the presentation
// "DirectX 11 Rendering in Battlefield 3" (2011) by Johan Andersson, DICE.
// Retrieved from: http://www.slideshare.net/DICEStudio/directx-11-rendering-in-battlefield-3
// Retrieved: July 13, 2015
// And "Forward+: A Step Toward Film-Style Shading in Real Time", Takahiro Harada (2012)
// published in "GPU Pro 4", Chapter 5 (2013) Taylor & Francis Group, LLC.
[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void CS_CullLights(uint3 groupID           : SV_GroupID,
                   uint3 groupThreadID     : SV_GroupThreadID,
                   uint3 dispatchThreadID  : SV_DispatchThreadID,
                   uint  groupIndex        : SV_GroupIndex)
{
    // Calculate min & max depth in threadgroup / tile.
    int2 texCoord = dispatchThreadID.xy;
    float fDepth = g_DepthTexture.Load(int3(texCoord, 0)).r;

	uint uDepth = asuint(fDepth);
    
    if (groupIndex == 0) // Avoid contention by other threads in the group.
    {
        gs_uMinDepth = 0xffffffff;
        gs_uMaxDepth = 0;
        gs_OpaqueLightCount = 0;
        gs_TransparentLightCount = 0;
        gs_GroupFrustum = g_PerTileFrustums[groupID.x + (groupID.y * numThreadGroups.x)];
    }
 
    GroupMemoryBarrierWithGroupSync();

    InterlockedMin(gs_uMinDepth, uDepth);
    InterlockedMax(gs_uMaxDepth, uDepth);
 
    GroupMemoryBarrierWithGroupSync();
    
	float fMinDepth = asfloat(gs_uMinDepth);
	float fMaxDepth = asfloat(gs_uMaxDepth);
 
    // Convert depth values to view space.
    float minDepthVS = ClipToView(float4(0.0f, 0.0f, fMinDepth, 1.0f)).z;
    float maxDepthVS = ClipToView(float4(0.0f, 0.0f, fMaxDepth, 1.0f)).z;
    float nearClipVS = ClipToView(float4(0.0f, 0.0f, 0.0f, 1.0f)).z;
 
    // Clipping plane for minimum depth value 
    // (used for testing lights within the bounds of opaque geometry).
    Plane minPlane = { float3(0.0f, 0.0f, 1.0f), minDepthVS };

    // Cull lights
    // Each thread in a group will cull 1 light until all lights have been culled.
    for (uint i = groupIndex; i < NUM_LIGHTS; i += TILE_SIZE * TILE_SIZE)
    {
        if (g_Lights[i].m_Active)
        {
            Light light = g_Lights[i];

            switch (light.m_Type)
            {
            case POINT_LIGHT:
            {
                Sphere sphere = { light.m_PositionVS.xyz, light.m_Range };
                if (SphereInsideFrustum(sphere, gs_GroupFrustum, nearClipVS, maxDepthVS))
                {
                    // Add light to light list for transparent geometry.
                    AppendLightForTransparentGeometry(i);
 
                    if (!SphereInsidePlane(sphere, minPlane))
                    {
                        // Add light to light list for opaque geometry.
                        AppendLightForOpaqueGeometry(i);
                    }
                }
                break;
            }
            case SPOT_LIGHT:
            {
                // TODO: It will already be in radians?
                float coneRadius = tan(radians(light.m_SpotlightAngle)) * light.m_Range;
                Cone cone = { light.m_PositionVS.xyz, light.m_Range, light.m_DirectionVS.xyz, coneRadius };
                if (ConeInsideFrustum(cone, gs_GroupFrustum, nearClipVS, maxDepthVS))
                {
                    // Add light to light list for transparent geometry.
                    AppendLightForTransparentGeometry(i);
 
                    if (!ConeInsidePlane(cone, minPlane))
                    {
                        // Add light to light list for opaque geometry.
                        AppendLightForOpaqueGeometry(i);
                    }
                }
                break;
            }
            case DIRECTIONAL_LIGHT:
            {
                // Directional lights always get added to our light list.
                // (Hopefully there are not too many directional lights!)
                AppendLightForOpaqueGeometry(i);
                AppendLightForTransparentGeometry(i);
                break;
            }
            // TODO: Capsule light (same as point light but with 2 spheres instead of one)
            }
        }
    }
 
    // Wait till all threads in group have caught up.
    GroupMemoryBarrierWithGroupSync();

    // Update global memory with visible light buffer.
    // First update the light grid (only thread 0 in group needs to do this)
    if (groupIndex == 0)
    {
        // Update light grid for opaque geometry.
        InterlockedAdd(g_OpaqueLightIndexCounter[0], gs_OpaqueLightCount, gs_OpaqueLightIndexStartOffset);
		g_OpaqueLightGrid[groupID.xy] = uint2(gs_OpaqueLightIndexStartOffset, gs_OpaqueLightCount);
 
        // Update light grid for transparent geometry.
		InterlockedAdd(g_TransparentLightIndexCounter[0], gs_TransparentLightCount, gs_TransparentLightIndexStartOffset);
		g_TransparentLightGrid[groupID.xy] = uint2(gs_TransparentLightIndexStartOffset, gs_TransparentLightCount);
	}
 
    GroupMemoryBarrierWithGroupSync();

    // Now update the light index list (all threads).
    // For opaque geometry.
	for (i = groupIndex; i < gs_OpaqueLightCount; i += TILE_SIZE * TILE_SIZE)
    {
		g_OpaqueLightIndexList[gs_OpaqueLightIndexStartOffset + i] = gs_OpaqueLightList[i];
	}
    // For transparent geometry.
	for (i = groupIndex; i < gs_TransparentLightCount; i += TILE_SIZE * TILE_SIZE)
    {
		g_TransparentLightIndexList[gs_TransparentLightIndexStartOffset + i] = gs_TransparentLightList[i];
	}
}