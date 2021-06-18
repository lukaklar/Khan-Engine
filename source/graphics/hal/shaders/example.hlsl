struct PS_INPUT
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD;
};

cbuffer CBUF : register(b0)
{
    float bound;
}

Texture2D g_Texture[5] : register(t0);
SamplerState g_Sampler : register(s0);

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 5; ++i)
    {
        input.texCoord.x += bound;
        finalColor *= g_Texture[0].Sample(g_Sampler, input.texCoord);
    }
    return finalColor;
}