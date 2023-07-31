#include "Shader_Client_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

texture2D g_TextureCircle; // ExplosionData
float3 g_vHpColor = float3(1.f, 0.f, 0.f);

float g_fDetail = 1.f;
float g_fTimeAcc = 1.f;
float g_fMaxHp = 0.f;
float g_fHp = 0.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix ViewProjMatrix = mul(g_ViewMatrix, g_ProjMatrix);
    matrix WVPMatrix = mul(g_WorldMatrix, ViewProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), WVPMatrix);
    Out.vTexUV = In.vTexUV;

    return Out;
}

VS_OUT VS_MAIN_LAVA(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix ViewProjMatrix = mul(g_ViewMatrix, g_ProjMatrix);
    matrix WVPMatrix = mul(g_WorldMatrix, ViewProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), WVPMatrix);
    
    Out.vTexUV.x = In.vTexUV.x + g_fTimeAcc;
    Out.vTexUV.y = In.vTexUV.y;

    return Out;
}

VS_OUT VS_MAIN_UI(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix ViewProjMatrix = mul(g_ViewMatrix, g_ProjMatrix);
    matrix WVPMatrix = mul(g_WorldMatrix, ViewProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), WVPMatrix);
    Out.vTexUV = In.vTexUV;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

float4 PS_MAIN(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV * g_fDetail);
	
    return vColor;
}

float4 PS_MAIN_UI(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
    if (vColor.a < 0.1f)
        discard;
	
    return vColor;
}

float4 PS_MAIN_HPBAR(PS_IN In) : SV_TARGET0
{
    float4 vHpBar = (float4) 0;
    
    vHpBar = g_Texture.Sample(LinearSampler, In.vTexUV);
    
    vHpBar = float4(g_vHpColor, 1.f);
    
    float fPer = g_fHp / g_fMaxHp;
    
    if (In.vTexUV.x > fPer)
    {
        vHpBar = float4(0.f, 0.f, 0.f, 1.f);
    }
    
    float4 vColor = (float4) 0;
    vColor = lerp(vColor, vHpBar, vHpBar.a);
    
    return vColor;
}

float4 PS_MAIN_EXPLOSION(PS_IN In) : SV_TARGET0
{
    vector vCircle = (vector) 0;
    
    vCircle = g_TextureCircle.Sample(LinearSampler, In.vTexUV);
    
    if (vCircle.a < 0.1f)
        discard;
    
    vCircle = vector(1.f, 0.f, 0.f, 0.5f);
    
    return vCircle;
}

float4 PS_MAIN_FADEIN(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
	
    vColor = vector(0.f, 0.f, 0.f, 1.f);
    
    vColor.a = saturate(vColor.a - (g_fTimeAcc - 1.6f));
    
    if (vColor.a < 0.01f)
        discard;
    
    return vColor;
}

float4 PS_MAIN_PHP(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
    
    float fPer = g_fHp / g_fMaxHp;
    
    if (In.vTexUV.x > fPer)
    {
        vColor = float4(0.f, 0.f, 0.f, 1.f);
    }
    
    float4 vRet = (float4) 0;
    vRet = lerp(vRet, vColor, vColor.a);
    
    return vRet;
}

technique11 DefaultTechnique
{
    pass BackGround //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Lava //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_LAVA();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_UI();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_UI();
    }

    pass HpBar //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_UI();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_HPBAR();
    }

    pass Explosion // 4 : 폭발 범위 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_EXPLOSION();
    }

    pass FadeIn // 5 : 페이드 인 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_FADEIN();
    }

    pass PlayerHP // 6: 플레이어 HP
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_PHP();
    }
};