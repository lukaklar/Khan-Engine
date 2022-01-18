#define NUM_HISTOGRAM_BINS 256
#define EPSILON 0.00000001f

#ifdef COMPUTE_HISTOGRAM

cbuffer LuminanceHistogramBuffer : register(b0)
{
    uint g_InputWidth;
    uint g_InputHeight;
    float g_MinLogLuminance;
    float g_OneOverLogLuminanceRange;
};

Texture2D g_HDRTexture : register(t0);

RWBuffer<uint> g_LuminanceHistogram : register(u0);

groupshared uint gs_Histogram[NUM_HISTOGRAM_BINS];

float GetLuminance(float3 color)
{
    return dot(color, float3(0.2127f, 0.7152f, 0.0722f));
}

uint HDRToHistogramBin(float3 hdrColor)
{
    float luminance = GetLuminance(hdrColor);
    
    if (luminance < EPSILON)
    {
        return 0;
    }
    
    float logLuminance = saturate((log2(luminance) - g_MinLogLuminance) * g_OneOverLogLuminanceRange);
    return (uint)(logLuminance * 254.0 + 1.0);
}

[numthreads(16, 16, 1)]
void CS_ComputeHistogram(uint groupIndex : SV_GroupIndex, uint3 dispatchThreadID : SV_DispatchThreadID)
{
    gs_Histogram[groupIndex] = 0;
    
    GroupMemoryBarrierWithGroupSync();

    if (dispatchThreadID.x < g_InputWidth && dispatchThreadID.y < g_InputHeight)
    {
        float3 hdrColor = g_HDRTexture.Load(int3(dispatchThreadID.xy, 0)).rgb;
        uint binIndex = HDRToHistogramBin(hdrColor);
        InterlockedAdd(gs_Histogram[binIndex], 1);
    }
    
    GroupMemoryBarrierWithGroupSync();

    InterlockedAdd(g_LuminanceHistogram[groupIndex], gs_Histogram[groupIndex]);
}

#endif

#ifdef AVERAGE_HISTOGRAM

Buffer<uint> g_LuminanceHistogram : register(t0);

RWBuffer<float> g_Luminance : register(u0);

cbuffer LuminanceHistogramAverageBuffer : register(b0)
{
    uint  g_PixelCount;
    float g_MinLogLuminance;
    float g_LogLuminanceRange;
    float g_TimeDelta;
    float g_Tau;
};

groupshared float gs_Histogram[NUM_HISTOGRAM_BINS];

[numthreads(16, 16, 1)]
void CS_AverageHistogram(uint groupIndex : SV_GroupIndex)
{
    float countForThisBin = (float)g_LuminanceHistogram[groupIndex];
    gs_Histogram[groupIndex] = countForThisBin * (float)groupIndex;
    
    GroupMemoryBarrierWithGroupSync();
    
    [unroll]
    for (uint histogramSampleIndex = (NUM_HISTOGRAM_BINS >> 1); histogramSampleIndex > 0; histogramSampleIndex >>= 1)
    {
        if (groupIndex < histogramSampleIndex)
        {
            gs_Histogram[groupIndex] += gs_Histogram[groupIndex + histogramSampleIndex];
        }

        GroupMemoryBarrierWithGroupSync();
    }
    
    if (groupIndex == 0)
    {
        float weightedLogAverage = (gs_Histogram[0] / max((float)g_PixelCount - countForThisBin, 1.0)) - 1.0;
        float weightedAverageLuminance = exp2(((weightedLogAverage / 254.0) * g_LogLuminanceRange) + g_MinLogLuminance);
        float luminanceLastFrame = g_Luminance[0];
        float adaptedLuminance = luminanceLastFrame + (weightedAverageLuminance - luminanceLastFrame) * (1 - exp(-g_TimeDelta * g_Tau));
        g_Luminance[0] = adaptedLuminance;
    }
}

#endif

#ifdef TONEMAP

//=================================================================================================
//
//  Baking Lab
//  by MJP and David Neubelt
//  http://mynameismjp.wordpress.com/
//
//  All code licensed under the MIT license
//
//=================================================================================================

// The code in this file was originally written by Stephen Hill (@self_shadow), who deserves all
// credit for coming up with this fit and implementing it. Buy him a beer next time you see him. :)

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
static const float3x3 ACESInputMat =
{
    float3(0.59719, 0.35458, 0.04823),
    float3(0.07600, 0.90834, 0.01566),
    float3(0.02840, 0.13383, 0.83777)
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 ACESOutputMat =
{
    float3(1.60475, -0.53108, -0.07367),
    float3(-0.10208, 1.10813, -0.00605),
    float3(-0.00327, -0.07276, 1.07602)
};

float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float3 ACESFitted(float3 color)
{
    color = mul(ACESInputMat, color);

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = mul(ACESOutputMat, color);

    // Clamp to [0, 1]
    color = saturate(color);

    return color;
}

Texture2D g_HDRInput : register(t0);
Buffer<float> g_AdaptedLuminance : register(t1);

RWTexture2D<float4> g_LDROutput : register(u0);

[numthreads(16, 16, 1)]
void CS_Tonemap(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int3 texCoord = int3(dispatchThreadID.xy, 0);
    
    float3 hdrColor = g_HDRInput.Load(texCoord).rgb;
    
    hdrColor *= g_AdaptedLuminance[0];
    
    float3 color = ACESFitted(hdrColor);
    
    color = pow(color, float3(0.4545, 0.4545, 0.4545));
    
    g_LDROutput[dispatchThreadID.xy] = float4(color, 1.0f);
}

#endif