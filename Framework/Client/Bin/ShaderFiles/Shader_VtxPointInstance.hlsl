#include "Shader_Client_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

vector g_vCamPosition;
float g_fLengthTexelU;
float g_fLengthTexelV;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vTexUV : TEXCOORD0;
	
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix WorldMatrix = matrix(In.vRight, In.vUp, In.vLook, In.vTranslation);
    Out.vPosition = mul(vector(In.vPosition, 1.f), WorldMatrix);

    Out.vPosition = mul(Out.vPosition, g_WorldMatrix);
    Out.vPSize = float2(In.vPSize.x * length(In.vRight), In.vPSize.y * length(In.vUp));
    Out.vTexUV = In.vTexUV;
	
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vTexUV : TEXCOORD0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];
	
    vector vLook = g_vCamPosition - In[0].vPosition;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight.xyz)) * In[0].vPSize.y * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
	
	/* 여기서 텍스처의 uv좌표를 0 ~ 1 사이값 중에 현재 스프라이트 위치에 맞게 변경 */

    Out[0].vPosition = mul(vector(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    Out[0].vTexUV = float2(g_fLengthTexelU * In[0].vTexUV.x, g_fLengthTexelV * In[0].vTexUV.y);

    Out[1].vPosition = mul(vector(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    Out[1].vTexUV = float2(g_fLengthTexelU * In[0].vTexUV.x + g_fLengthTexelU, g_fLengthTexelV * In[0].vTexUV.y);

    Out[2].vPosition = mul(vector(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
    Out[2].vTexUV = float2(g_fLengthTexelU * In[0].vTexUV.x + g_fLengthTexelU, g_fLengthTexelV * In[0].vTexUV.y + g_fLengthTexelV);

    Out[3].vPosition = mul(vector(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
    Out[3].vTexUV = float2(g_fLengthTexelU * In[0].vTexUV.x, g_fLengthTexelV * In[0].vTexUV.y + g_fLengthTexelV);
	
    Triangles.Append(Out[0]);
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.RestartStrip();
	
    Triangles.Append(Out[0]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[3]);
    Triangles.RestartStrip();
}

[maxvertexcount(6)]
void GS_MAIN_T(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];
	
    vector vLook = g_vCamPosition - In[0].vPosition;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight.xyz)) * In[0].vPSize.y * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
	
	/* 여기서 텍스처의 uv좌표를 0 ~ 1 사이값 중에 현재 스프라이트 위치에 맞게 변경 */

    Out[0].vPosition = mul(vector(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    Out[0].vTexUV = float2(0.f, 0.f);

    Out[1].vPosition = mul(vector(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    Out[1].vTexUV = float2(1.f, 0.f);

    Out[2].vPosition = mul(vector(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
    Out[2].vTexUV = float2(1.f, 1.f);

    Out[3].vPosition = mul(vector(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
    Out[3].vTexUV = float2(0.f, 1.f);
	
    Triangles.Append(Out[0]);
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.RestartStrip();
	
    Triangles.Append(Out[0]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[3]);
    Triangles.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

float4 PS_MAIN(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	
    if (vColor.r < 0.1f && vColor.g < 0.1f && vColor.b < 0.1f)
        vColor.a = 0.f;
    
    if (vColor.a < 0.1f)
        discard;
	
    return vColor;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Test
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_T();
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
};