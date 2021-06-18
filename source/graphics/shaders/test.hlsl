float2 positions[3] =
{
    { 0.0, -0.5 },
    { 0.5, 0.5 },
    { -0.5, 0.5 }
};

float3 colors[3] =
{
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 }
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 Color;
};

VS_OUTPUT VS_Main()
{
    VS_OUTPUT Out;
    Out.Position = float4(positions[SV_VertexID], 0.0f, 1.0f);
    Out.Color = colors[SV_VertexID];

    return Out;
}

float4 PS_Main(VS_OUTPUT In) : SV_Target
{
    return float4(In.Color, 1.0f);
}