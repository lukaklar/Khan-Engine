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

VS_TO_PS VS_Test(float3 position : POSITION, float3 color : COLOR0)
{
    VS_TO_PS Out;
    Out.Position = mul(m_ViewProj, mul(m_ModelMatrix, float4(position, 1.0f)));
    Out.Color = color;

    return Out;
}

float4 PS_Test(VS_TO_PS In) : SV_Target
{
    return float4(In.Color, 1.0f);
}