cbuffer FrustumParams : register(b0)
{
    float4x4 g_Projection;
    float4x4 g_InverseProjection;
    float2   g_ScreenDimensions;
    float    g_Near;
    float    g_Far;
    float3   g_ClusterCount;
}

Texture2D g_InputTexture : register(t0);

RWTexture2D<float4> g_OutputTexture : register(u0);

static const SamplerState g_LinearSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};


[numthreads(16, 16, 1)]
void CS_FXAAFilter(uint3 groupID           : SV_GroupID,
                   uint3 groupThreadID     : SV_GroupThreadID,
                   uint3 dispatchThreadID  : SV_DispatchThreadID,
                   uint  groupIndex        : SV_GroupIndex)
{
    const float FXAA_SPAN_MAX = 8.0f;
    const float FXAA_REDUCE_MUL = 1.0f / 8.0f;
    const float FXAA_REDUCE_MIN = 1.0f / 128.0f;
    
    float2 texCoord = dispatchThreadID.xy / g_ScreenDimensions;
    
    float3 rgbNW = g_InputTexture.SampleLevel(g_LinearSampler, texCoord + float2(-1.0f, -1.0f) / g_ScreenDimensions, 0).xyz;
    float3 rgbNE = g_InputTexture.SampleLevel(g_LinearSampler, texCoord + float2(1.0f, -1.0f) / g_ScreenDimensions, 0).xyz;
    float3 rgbSW = g_InputTexture.SampleLevel(g_LinearSampler, texCoord + float2(-1.0f, 1.0f) / g_ScreenDimensions, 0).xyz;
    float3 rgbSE = g_InputTexture.SampleLevel(g_LinearSampler, texCoord + float2(1.0f, 1.0f) / g_ScreenDimensions, 0).xyz;
    float3 rgbM = g_InputTexture.SampleLevel(g_LinearSampler, texCoord, 0).xyz;

    const float3 luma = float3(0.299f, 0.587f, 0.114f);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    float2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25f * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

    float rcpDirMin = 1.0f / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(float2(FXAA_SPAN_MAX,  FXAA_SPAN_MAX), max(float2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) / g_ScreenDimensions;

    float3 rgbA = (1.0f / 2.0f) * (g_InputTexture.SampleLevel(g_LinearSampler, texCoord + dir * (1.0f / 3.0f - 0.5f), 0).xyz + g_InputTexture.SampleLevel(g_LinearSampler, texCoord + dir * (2.0f / 3.0f - 0.5f), 0).xyz);
    float3 rgbB = rgbA * (1.0f / 2.0f) + (1.0f / 4.0f) * (g_InputTexture.SampleLevel(g_LinearSampler, texCoord + dir * (0.0f / 3.0f - 0.5f), 0).xyz + g_InputTexture.SampleLevel(g_LinearSampler, texCoord + dir * (3.0f / 3.0f - 0.5f), 0).xyz);
    float lumaB = dot(rgbB, luma);

    if (lumaB < lumaMin || lumaB > lumaMax)
    {
        g_OutputTexture[dispatchThreadID.xy] = float4(rgbA, 0.0f);
    }
    else
    {
        g_OutputTexture[dispatchThreadID.xy] = float4(rgbB, 0.0f);
    }
}