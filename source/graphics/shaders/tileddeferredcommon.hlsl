#define TILE_SIZE 16

#define NUM_LIGHTS 8

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2
#define CAPSULE_LIGHT 3

struct Light
{
	float4 m_Position1WS; // world space position for point, spot and capsule lights
	float4 m_Position2WS; // world space position only for capsule lights
	float4 m_DirectionWS; // world space direction for directional, spot and capsule lights
	float4 m_Position1VS; // view space position for point, spot and capsule lights
	float4 m_Position2VS; // view space position only for capsule lights
	float4 m_DirectionVS; // view space direction for directional, spot and capsule lights
	float3 m_Color;
	float  m_Luminance;
	float  m_SpotlightAngle;
	float  m_Range;
	float  m_CapsuleLength;
	uint   m_Type;
	bool   m_Active;
};

struct Plane
{
    float3 m_Normal;   // Plane normal.
    float  m_Distance;   // Distance to origin.
};

// Four planes of a view frustum (in view space).
// The planes are:
//  * Left,
//  * Right,
//  * Top,
//  * Bottom.
// The back and/or front planes can be computed from depth values in the 
// light culling compute shader.
struct Frustum
{
    Plane m_Planes[4];   // left, right, top, bottom frustum planes.
};

// Compute a plane from 3 noncollinear points that form a triangle.
// This equation assumes a right-handed (counter-clockwise winding order) 
// coordinate system to determine the direction of the plane normal.
Plane ComputePlane(float3 p0, float3 p1, float3 p2)
{
    Plane plane;
 
    float3 v0 = p1 - p0;
    float3 v2 = p2 - p0;
 
    plane.m_Normal = normalize(cross(v0, v2));
 
    // Compute the distance to the origin using p0.
    plane.m_Distance = dot(plane.m_Normal, p0);
 
    return plane;
}

// Global variables
cbuffer DispatchParams : register(b0)
{
    // Number of groups dispatched. (This parameter is not available as an HLSL system value!)
    uint3 numThreadGroups;
    // uint padding // implicit padding to 16 bytes.
 
    // Total number of threads dispatched. (Also not available as an HLSL system value!)
    // Note: This value may be less than the actual number of threads executed 
    // if the screen size is not evenly divisible by the block size.
    uint3 numThreads;
    // uint padding // implicit padding to 16 bytes.
}

// Parameters required to convert screen space coordinates to view space.
cbuffer ScreenToViewParams : register(b1)
{
    float4x4 InverseProjection;
    float2 ScreenDimensions;
}

// Convert clip space coordinates to view space
float4 ClipToView(float4 clip)
{
    // View space position.
    float4 view = mul(InverseProjection, clip);
    // Perspective projection.
    view = view / view.w;
 
    return view;
}

// Convert screen space coordinates to view space.
float4 ScreenToView(float4 screen)
{
    // Convert to normalized texture coordinates
    float2 texCoord = screen.xy / ScreenDimensions;
 
    // Convert to clip space
    float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);
 
    return ClipToView(clip);
}