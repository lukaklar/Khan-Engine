#define TILE_SIZE 16
#define NUM_DEPTH_SLICES 32

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

struct Cluster
{
    float3 m_AABBMin;
	float  m_Padding1;
    float3 m_AABBMax;
	float  m_Padding2;
};