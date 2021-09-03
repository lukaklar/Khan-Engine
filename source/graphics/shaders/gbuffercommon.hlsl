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
    float4 m_Position : SV_Position;
    float2 m_TexCoord : TEXCOORD;
    float3x3 m_TBN    : TBN;
};

cbuffer PerFrameConsts : register(b0, space0)
{
    float4x4 g_ViewProjection;
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
    
    float3 T = normalize(mul(g_ModelMatrix, float4(In.m_Tangent, 0.0)).xyz);
    float3 B = normalize(mul(g_ModelMatrix, float4(In.m_Bitangent, 0.0)).xyz);
    float3 N = normalize(mul(g_ModelMatrix, float4(In.m_Normal, 0.0)).xyz);
    Out.m_TBN = float3x3(T.x, B.x, N.x, T.y, B.y, N.y, T.z, B.z, N.z);
    
    return Out;
}

struct PS_OUT
{
    float4 m_Albedo               : SV_Target0;
    float3 m_Normal               : SV_Target1;
    float3 m_Emissive             : SV_Target2;
    float4 m_Specular             : SV_Target3;
    float2 m_MetallicAndRoughness : SV_Target4;
};

static const SamplerState g_DefaultSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D g_Diffuse : register(t0, space1);
Texture2D g_Specular : register(t1, space1);
Texture2D g_Normals : register(t2, space1);

PS_OUT PS_Common(VS_TO_PS In)
{
    PS_OUT Out;
    
    Out.m_Albedo = g_Diffuse.Sample(g_DefaultSampler, In.m_TexCoord);
    Out.m_Normal = normalize(mul(In.m_TBN, ((g_Normals.Sample(g_DefaultSampler, In.m_TexCoord).xyz) * 2.0 - 1.0)));
    Out.m_Specular = g_Specular.Sample(g_DefaultSampler, In.m_TexCoord);
    Out.m_Emissive = float3(0.0, 0.0, 0.0);
    Out.m_MetallicAndRoughness = float2(0.0, 0.5);
    
    return Out;
}

PS_OUT PS_CommonNoNormals(VS_TO_PS In)
{
    PS_OUT Out;
    
    Out.m_Albedo = g_Diffuse.Sample(g_DefaultSampler, In.m_TexCoord);
    Out.m_Normal = float3(0.0, 0.0, 0.0);
    Out.m_Specular = g_Specular.Sample(g_DefaultSampler, In.m_TexCoord);
    Out.m_Emissive = float3(0.0, 0.0, 0.0);
    Out.m_MetallicAndRoughness = float2(0.0, 0.5);
    
    return Out;
}

PS_OUT PS_CommonDiffuseOnly(VS_TO_PS In)
{
    PS_OUT Out;
    
    Out.m_Albedo = g_Diffuse.Sample(g_DefaultSampler, In.m_TexCoord);
    Out.m_Normal = float3(0.0, 0.0, 0.0);
    Out.m_Specular = Out.m_Albedo;
    Out.m_Emissive = float3(0.0, 0.0, 0.0);
    Out.m_MetallicAndRoughness = float2(0.0, 0.5);
    
    return Out;
}

PS_OUT PS_GBufferTest(VS_TO_PS In)
{
    PS_OUT Out;
    
    Out.m_Albedo = float4(In.m_TexCoord, 0.0f, 1.0f);
    Out.m_Normal = float3(0.0f, 1.0f, 0.0f);
    Out.m_Specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
    Out.m_Emissive = float3(0.0f, 0.0f, 0.0f);
    Out.m_MetallicAndRoughness = float2(0.5f, 0.5f);
    
    return Out;
}