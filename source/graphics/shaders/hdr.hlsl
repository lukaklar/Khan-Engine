cbuffer DownScaleConstants : register(b0)
{
    // Resolution of the down scaled target: x - width, y - height
    uint2 g_Resolution;
    // Total pixel in the downscaled image
    uint g_Domain;
    // Number of groups dispached on the first pass
    uint g_GroupSize;
}

Texture2D g_HDRTex : register(t0);
RWStructuredBuffer<float> g_IntermediateLuminanceValues : register(u0);

groupshared float gs_SharedPositions[1024];

static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

float DownScale4x4(uint2 currentPixel, uint groupThreadID)
{
    float avgLum = 0.0f;
    
    if (currentPixel.y < g_Resolution.y)
    {
        int3 fullResPos = int3(currentPixel * 4, 0);
        float4 downScaled = float4(0.0, 0.0, 0.0, 0.0);
        
        [unroll]
        for (int i = 0; i < 4; i++)
        {
            [unroll]
            for (int j = 0; j < 4; j++)
            {
                downScaled += g_HDRTex.Load(fullResPos, int2(j, i));
            }
        }
        downScaled /= 16.0;
        
        // Calculate the luminance value for this pixel
        avgLum = dot(downScaled, LUM_FACTOR);
        
        // Write the result to the shared memory
        gs_SharedPositions[groupThreadID] = avgLum;
    }
    
    // Synchronize before next step
    GroupMemoryBarrierWithGroupSync();

    return avgLum;
}

float DownScale1024to4(uint dispatchThreadID, uint groupThreadID, float avgLum)
{
    // Expand the downscale code from a loop
    [unroll]
    for (uint groupSize = 4, step1 = 1, step2 = 2, step3 = 3; groupSize < 1024; groupSize *= 4, step1 *= 4, step2 *= 4, step3 *= 4)
    {
        // Skip out of bound pixels
        if (groupThreadID % groupSize == 0)
        {
            // Calculate the luminance sum for this step
            float stepAvgLum = avgLum;
            stepAvgLum += dispatchThreadID + step1 < g_Domain ? gs_SharedPositions[groupThreadID + step1] : avgLum;
            stepAvgLum += dispatchThreadID + step1 < g_Domain ? gs_SharedPositions[groupThreadID + step2] : avgLum;
            stepAvgLum += dispatchThreadID + step1 < g_Domain ? gs_SharedPositions[groupThreadID + step3] : avgLum;
            
            // Store the results
            avgLum = stepAvgLum;
            gs_SharedPositions[groupThreadID] = stepAvgLum;
        }
        
        // Synchronize before next step
        GroupMemoryBarrierWithGroupSync();
    }
    
    return avgLum;
}

void DownScale4to1(uint dispatchThreadID, uint groupThreadID, uint groupID, float avgLum)
{
    if (groupThreadID == 0)
    {
        // Calculate the average lumenance for this thread group
        float finalAvgLum = avgLum;
        finalAvgLum += dispatchThreadID + 256 < g_Domain ? gs_SharedPositions[groupThreadID + 256] : avgLum;
        finalAvgLum += dispatchThreadID + 512 < g_Domain ? gs_SharedPositions[groupThreadID + 512] : avgLum;
        finalAvgLum += dispatchThreadID + 768 < g_Domain ? gs_SharedPositions[groupThreadID + 768] : avgLum;
        finalAvgLum /= 1024.0;
        
        // Write the final value into the 1D UAV which
        // will be used on the next step
        g_IntermediateLuminanceValues[groupID] = finalAvgLum;
    }
}

[numthreads(1024, 1, 1)]
void CS_DownScalePass1(uint3 groupID           : SV_GroupID,
                       uint3 groupThreadID     : SV_GroupThreadID,
                       uint3 dispatchThreadID  : SV_DispatchThreadID,
                       uint  groupIndex        : SV_GroupIndex)
{
    uint2 currentPixel = uint2(dispatchThreadID.x % g_Resolution.x, dispatchThreadID.x / g_Resolution.x);
    
    // Reduce a group of 16 pixels to a single pixel and store in the shared memory
    float avgLum = DownScale4x4(currentPixel, groupThreadID.x);
    
    // Down scale from 1024 to 4
    avgLum = DownScale1024to4(dispatchThreadID.x, groupThreadID.x, avgLum);
    
    // Downscale from 4 to 1
    DownScale4to1(dispatchThreadID.x, groupThreadID.x, groupID.x, avgLum);
}

#define MAX_GROUPS 64

StructuredBuffer<float> g_IntermediateValues : register(t0);
RWStructuredBuffer<float> g_AverageLuminance : register(u0);

// Group shared memory to store the intermediate results
groupshared float gs_SharedAvgFinal[MAX_GROUPS];

[numthreads(MAX_GROUPS, 1, 1)]
void CS_DownScalePass2(uint3 groupID           : SV_GroupID,
                       uint3 groupThreadID     : SV_GroupThreadID,
                       uint3 dispatchThreadID  : SV_DispatchThreadID,
                       uint  groupIndex        : SV_GroupIndex)
{
    // Fill the shared memory with the 1D values
    float avgLum = 0.0;
    if (dispatchThreadID.x < g_GroupSize)
    {
        avgLum = g_IntermediateValues[dispatchThreadID.x];
    }
    
    gs_SharedAvgFinal[dispatchThreadID.x] = avgLum;
    
    GroupMemoryBarrierWithGroupSync(); // Sync before next step
    
    // Downscale from 64 to 16
    if(dispatchThreadID.x % 4 == 0)
    {
        // Calculate the luminance sum for this step
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadID.x+1 < g_GroupSize ? gs_SharedAvgFinal[dispatchThreadID.x + 1] : avgLum;
        stepAvgLum += dispatchThreadID.x+2 < g_GroupSize ? gs_SharedAvgFinal[dispatchThreadID.x + 2] : avgLum;
        stepAvgLum += dispatchThreadID.x+3 < g_GroupSize ? gs_SharedAvgFinal[dispatchThreadID.x + 3] : avgLum;

        // Store the results
        avgLum = stepAvgLum;
        gs_SharedAvgFinal[dispatchThreadID.x] = stepAvgLum;
    }
    
    GroupMemoryBarrierWithGroupSync(); // Sync before next step
    
    // Downscale from 16 to 4
    if(dispatchThreadID.x % 16 == 0)
    {
        // Calculate the luminance sum for this step
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadID.x + 4 < g_GroupSize ? gs_SharedAvgFinal[dispatchThreadID.x + 4] : avgLum;
        stepAvgLum += dispatchThreadID.x  +8 < g_GroupSize ? gs_SharedAvgFinal[dispatchThreadID.x + 8] : avgLum;
        stepAvgLum += dispatchThreadID.x + 12 < g_GroupSize ? gs_SharedAvgFinal[dispatchThreadID.x + 12] : avgLum;
        
        // Store the results
        avgLum = stepAvgLum;
        gs_SharedAvgFinal[dispatchThreadID.x] = stepAvgLum;
    }
    
    GroupMemoryBarrierWithGroupSync(); // Sync before next step
    
    // Downscale from 4 to 1
    if (dispatchThreadID.x == 0)
    {
        // Calculate the average luminace
        float finalLumValue = avgLum;
        finalLumValue += dispatchThreadID.x + 16 < g_GroupSize ? gs_SharedAvgFinal[dispatchThreadID.x + 16] : avgLum;
        finalLumValue += dispatchThreadID.x + 32 < g_GroupSize ? gs_SharedAvgFinal[dispatchThreadID.x + 32] : avgLum;
        finalLumValue += dispatchThreadID.x + 48 < g_GroupSize ? gs_SharedAvgFinal[dispatchThreadID.x + 48] : avgLum;
        finalLumValue /= 64.0;
        g_AverageLuminance[0] = finalLumValue;
    }
}

cbuffer TonemapConstants : register(b0)
{
    float g_MiddleGrey;
    float g_LumWhiteSqr;
}

StructuredBuffer<float> g_AvgLum : register(t1);
RWTexture2D<float4> g_HDROutput : register(u0);

float3 ToneMapping(float3 HDRColor)
{
    // Find the luminance scale for the current pixel
    float LScale = dot(HDRColor, LUM_FACTOR.xyz);
    LScale *= g_MiddleGrey / g_AvgLum[0];
    LScale = (LScale + LScale * LScale / g_LumWhiteSqr) / (1.0 + LScale);
    
    // Apply the luminance scale to the pixels color
    return HDRColor * LScale;
}

[numthreads(32, 32, 1)]
void CS_TonemapPass(uint3 dispatchThreadID  : SV_DispatchThreadID)
{
    int3 texCoord = int3(dispatchThreadID.xy, 0);
    
    float3 color = g_HDRTex.Load(texCoord).xyz;
    
    // Tone mapping
    color = ToneMapping(color);
    
    // TODO: Write the color back to the texture
    g_HDROutput[texCoord.xy] = float4(color, 1.0);
}