#include "deferredcommon.hlsl"

cbuffer FrustumParams : register(b0)
{
    float4x4 g_Projection;
    float4x4 g_InverseProjection;
    float2   g_ScreenDimensions;
    float    g_Near;
    float    g_Far;
    float3   g_ClusterCount;
}

cbuffer SamplingParams : register(b1)
{
    float3 g_Samples[64];
    float3 g_Noise[16];
}

float4 ClipToView(float4 clip)
{
    float4 view = mul(g_InverseProjection, clip);
    
    view = view / view.w;
    
    return view;
}

float4 ScreenToView(float4 screen)
{
    float2 texCoord = screen.xy / g_ScreenDimensions;
    
    float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);
    
    return ClipToView(clip);
}

Texture2D g_GBuffer_Normals : register(t0);
Texture2D g_GBuffer_Depth   : register(t1);

RWTexture2D<float> g_OcclusionFactors : register(u0);

static const int g_KernelSize = 64;
static const float g_Radius = 0.5;
static const float g_Bias = 0.025;

[numthreads(16, 16, 1)]
void CS_SSAOCalculate(uint3 groupID           : SV_GroupID,
                      uint3 groupThreadID     : SV_GroupThreadID,
                      uint3 dispatchThreadID  : SV_DispatchThreadID,
                      uint  groupIndex        : SV_GroupIndex)
{
    int3 texCoord = float3(dispatchThreadID.xy, 0);
    
    float depth = g_GBuffer_Depth.Load(texCoord).r;
    float3 positionVS = ScreenToView(float4(texCoord.xy, depth, 1.0f)).xyz;
    float3 normalVS = DecodeNormal(g_GBuffer_Normals.Load(texCoord).rg);
    float3 randomVec = g_Noise[(groupThreadID.y & 3) * 4 + groupThreadID.x & 3];
    
    float3 tangent = normalVS; //normalize(randomVec - normalVS * dot(randomVec, normalVS));
    float3 bitangent = cross(normalVS, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normalVS);
    
    float occlusion = 0.0f;
    for (int i = 0; i < g_KernelSize; ++i)
    {
        float3 samplePos = mul(g_Samples[i], TBN);
        samplePos = positionVS + samplePos * g_Radius;
        
        float4 offset = float4(samplePos, 1.0f);
        offset = mul(g_Projection, offset);
        offset.xy /= offset.w;
        offset.xy = clamp(float2(offset.x + 1.0f, 1.0f - offset.y) * 0.5f * g_ScreenDimensions, float2(0.0f, 0.0f), g_ScreenDimensions - 1.0f);

        float sampleDepth = g_GBuffer_Depth.Load(int3(offset.xy, 0)).r;
        sampleDepth = ScreenToView(float4(offset.xy, sampleDepth, 1.0f)).z;
        
        float rangeCheck = smoothstep(0.0, 1.0, g_Radius / abs(positionVS.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + g_Bias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0f - (occlusion / g_KernelSize);
    
    g_OcclusionFactors[dispatchThreadID.xy] = occlusion;
}

static const int g_BlurSize = 4;

static const SamplerState g_PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

Texture2D<float> g_SSAOTexture : register(t0);

RWTexture2D<float> g_BlurredSSAO : register(u0);

[numthreads(16, 16, 1)]
void CS_SSAOBlur(uint3 groupID           : SV_GroupID,
                 uint3 groupThreadID     : SV_GroupThreadID,
                 uint3 dispatchThreadID  : SV_DispatchThreadID,
                 uint  groupIndex        : SV_GroupIndex)
{
    float2 texCoord = dispatchThreadID.xy / g_ScreenDimensions;
    float2 texelSize = 1.0f / g_ScreenDimensions;
    
    float result = 0.0f;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            float2 offset = float2(float(x), float(y)) * texelSize;
            result += g_SSAOTexture.SampleLevel(g_PointSampler, texCoord + offset, 0);
        }
    }
    
    g_BlurredSSAO[dispatchThreadID.xy] = result / (4.0f * 4.0f);
}