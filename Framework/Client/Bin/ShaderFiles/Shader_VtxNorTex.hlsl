#include "Shader_Client_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture[10];
texture2D g_NormalTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT_PHONG
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

VS_OUT_PHONG VS_MAIN_PHONG(VS_IN In)
{
    VS_OUT_PHONG Out = (VS_OUT_PHONG) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    vector vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vNormal = normalize(vWorldNormal);
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    if (0 == length(cross(float3(0.f, 1.f, 0.f), Out.vNormal.xyz)))
    {
        Out.vTangent = vector(float3(1.f, 0.f, 0.f), 0.f);
        Out.vBinormal = vector(float3(0.f, 0.f, 1.f), 0.f);
    }
    else
    {
        Out.vTangent = normalize(vector(cross(float3(0.f, 1.f, 0.f), normalize(Out.vNormal.xyz)), 0.f));
        Out.vBinormal = normalize(vector(cross(normalize(Out.vNormal.xyz), Out.vTangent.xyz), 0.f));
    }
    
    return Out;
}

struct PS_IN_PHONG
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};


struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT PS_MAIN_PHONG(PS_IN_PHONG In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture[2].Sample(PointSampler, In.vTexUV * 10.f);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV * 10.f);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vDiffuse = vDiffuse;
    Out.vDiffuse.a = 1.f;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}


technique11 DefaultTechnique
{
    pass Terrain_Phong
    {
        SetRasterizerState( /*RS_Wireframe*/RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_PHONG();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_PHONG();
    }
}