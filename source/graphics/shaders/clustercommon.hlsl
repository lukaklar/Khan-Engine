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
    float3 m_Attenuation;
    float  m_InnerConeAngle;
    float  m_OuterConeAngle;
    float3 m_Padding;
};

struct Cluster
{
    float3 m_AABBMin;
	float  m_Padding1;
    float3 m_AABBMax;
	float  m_Padding2;
};