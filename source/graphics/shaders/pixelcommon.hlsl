static const float2 positions[3] =
{
    { 0.0, -0.5 },
    { 0.5, 0.5 },
    { -0.5, 0.5 }
};

static const float3 colors[3] =
{
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 }
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 Color    : COLOR;
};

VS_OUTPUT VS_Main(uint vertexID : SV_VertexID)
{
    VS_OUTPUT Out;
    Out.Position = float4(positions[vertexID], 0.0f, 1.0f);
    Out.Color = colors[vertexID];

    return Out;
}

float4 PS_Main(VS_OUTPUT In) : SV_Target
{
    return float4(In.Color, 1.0f);
}