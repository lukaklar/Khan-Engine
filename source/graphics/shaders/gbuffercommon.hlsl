struct VS_IN
{
    float3 m_Position  : POSITION;
    float2 m_TexCoord  : TEXCOORD0;
    float3 m_Normal    : NORMAL;
    float3 m_Tangent   : TANGENT;
    float3 m_Bitangent : BITANGENT;
};

struct VS_TO_PS
{
    float4 m_Position       : SV_POSITION;
    float2 m_TexCoord       : TEXCOORD;
    float3 m_NormalVS       : NORMAL;
    float3 m_TangentVS      : TANGENT;
    float3 m_BitangentVS    : BITANGENT;
};

cbuffer PerFrameConsts : register(b0, space0)
{
    float4x4 g_ViewProjection;
    float4x4 g_ViewMatrix;
}

cbuffer PerDrawConsts : register(b0, space2)
{
    float4x4 g_ModelMatrix;
}

VS_TO_PS VS_Common(VS_IN In)
{
    VS_TO_PS Out;
    
    Out.m_Position = mul(g_ViewProjection, mul(g_ModelMatrix, float4(In.m_Position, 1.0)));
    Out.m_TexCoord = In.m_TexCoord;
    float3x3 modelView = (float3x3) mul(g_ViewMatrix, g_ModelMatrix);
    Out.m_NormalVS = mul(modelView, In.m_Normal);
    Out.m_TangentVS = mul(modelView, In.m_Tangent);
    Out.m_BitangentVS = mul(modelView, In.m_Bitangent);
    
    return Out;
}

struct PS_OUT
{
    float4 m_Albedo     : SV_Target0;
    float3 m_Normal     : SV_Target1;
    float3 m_Emissive   : SV_Target2;
    float2 m_PBRConsts  : SV_Target3; // metallic and rougness
};

static const SamplerState g_DefaultSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D g_DiffuseTexture : register(t0, space1);
Texture2D g_NormalsTexture : register(t1, space1);
Texture2D g_MetalnessTexture : register(t2, space1);
Texture2D g_RoughnessTexture : register(t3, space1);

PS_OUT PS_Common(VS_TO_PS In)
{
    PS_OUT Out;
    
    float3x3 TBN = float3x3(normalize(In.m_TangentVS),
                            normalize(In.m_BitangentVS),
                            normalize(In.m_NormalVS));
    
    Out.m_Albedo = g_DiffuseTexture.Sample(g_DefaultSampler, In.m_TexCoord);
    Out.m_Normal = normalize(mul((g_NormalsTexture.Sample(g_DefaultSampler, In.m_TexCoord).xyz * 2.0f - 1.0f), TBN)) * 0.5f + 0.5f;
    Out.m_Emissive = float3(0.0f, 0.0f, 0.0f);
    Out.m_PBRConsts = float2(g_MetalnessTexture.Sample(g_DefaultSampler, In.m_TexCoord).x, g_RoughnessTexture.Sample(g_DefaultSampler, In.m_TexCoord).x);

    
    return Out;
}

PS_OUT PS_CommonNoNormals(VS_TO_PS In)
{
    PS_OUT Out;
    
    Out.m_Albedo = g_DiffuseTexture.Sample(g_DefaultSampler, In.m_TexCoord);
    Out.m_Normal = float3(0.0, 0.0, 0.0);
    Out.m_Emissive = float3(0.0, 0.0, 0.0);
    Out.m_PBRConsts = float2(g_MetalnessTexture.Sample(g_DefaultSampler, In.m_TexCoord).x, 0.5f);
    
    return Out;
}

PS_OUT PS_CommonDiffuseOnly(VS_TO_PS In)
{
    PS_OUT Out;
    
    Out.m_Albedo = g_DiffuseTexture.Sample(g_DefaultSampler, In.m_TexCoord);
    Out.m_Normal = float3(0.0, 0.0, 0.0);
    Out.m_Emissive = float3(0.0, 0.0, 0.0);
    Out.m_PBRConsts = float2(1.0, 0.0);
    
    return Out;
}

PS_OUT PS_GBufferTest(VS_TO_PS In)
{
    PS_OUT Out;
    
    Out.m_Albedo = float4(In.m_TexCoord, 0.0f, 1.0f);
    Out.m_Normal = float3(0.0f, 1.0f, 0.0f);
    Out.m_Emissive = float3(0.0f, 0.0f, 0.0f);
    Out.m_PBRConsts = float2(0.5f, 0.5f);
    
    return Out;
}