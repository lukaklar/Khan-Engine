struct VS_INPUT
{
    float3 m_Position   : POSITION;
    float3 m_TexCoord   : TEXCOORD;
    float3 m_Normal     : NORMAL;       // acting as color for now
    float3 m_Tangent    : TANGENT;
    float3 m_Bitangent  : BITANGENT;
};

struct VS_TO_PS
{
    float4 Position : SV_Position;
    float3 Color    : COLOR;
};

cbuffer PerFrameConsts : register(b0, space0)
{
    float4x4 m_ViewProj;
};

cbuffer PerDrawConsts : register(b0, space2)
{
    float4x4 m_ModelMatrix;
};

VS_TO_PS VS_Test(VS_INPUT In)
{
    VS_TO_PS Out;
    Out.Position = mul(m_ViewProj, mul(m_ModelMatrix, float4(In.m_Position, 1.0f)));
    Out.Color = In.m_Normal;

    return Out;
}

float4 PS_Test(VS_TO_PS In) : SV_Target
{
    return float4(In.Color, 1.0f);
}