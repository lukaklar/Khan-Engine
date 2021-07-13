struct VS_INPUT
{
    float3 m_Position   : POSITION;
    float3 m_TexCoord   : TEXCOORD;
    float3 m_Normal     : NORMAL;
    float3 m_Bitangent  : BITANGENT;
    float3 m_Tangent    : TANGENT;
};

cbuffer PerFrame : register(b0, space0)
{
    float4x4 g_ViewProjection;
};

cbuffer PerObject : register(b0, space2)
{
    float4x4 g_ModelMatrix;
};

float4 VS_Main(VS_INPUT In) : SV_Position
{
    float4 position = float4(In.m_Position, 1.0);

    return mul(g_ViewProjection, mul(g_ModelMatrix, position));
}