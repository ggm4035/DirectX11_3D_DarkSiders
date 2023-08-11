#include "Shader_Client_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture, g_AlphaTexture;

texture2D g_TextureCircle; // ExplosionData
float3 g_vHpColor = float3(1.f, 0.f, 0.f);

float g_fDetail = 1.f;
float g_fTimeAcc = 1.f;
float g_fCoolTime = 0.f;
float g_fHpPer = 0.f;
float g_fHeightNum = 1.f;
float g_fWidthNum = 1.f;
float g_fLimitTime = 1.f;

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
    float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
    
    float fVal = (1.f - g_fHpPer) * 0.8f;
    vColor.r += fVal;
    vColor.gb -= fVal * 0.5f;
    
    if (In.vTexUV.x > g_fHpPer)
    {
        vColor = float4(0.f, 0.f, 0.f, 1.f);
    }
    
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
    
    float fVal = (1.f - g_fHpPer) * 0.8f;
    vColor.r += fVal;
    vColor.gb -= fVal * 0.5f;
    
    if (In.vTexUV.x > g_fHpPer)
    {
        vColor = float4(0.f, 0.f, 0.f, 1.f);
    }
    
    return vColor;
}

float4 PS_MAIN_COOLTIME(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
    if (vColor.a < 0.1f)
        discard;
    
    if (In.vTexUV.y > g_fCoolTime) // 0 ~ 1
    {
        vColor.rgb = (vColor.r + vColor.g + vColor.b) / 3.f;
    }
	
    return vColor;
}

float4 PS_MAIN_COLOR(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
    
    vColor.a = vColor.r;
    
    if (vColor.a < 0.1f)
        discard;
    
    return vColor;
}

float4 PS_MAIN_SPRITE(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
    
    if (vColor.r < 0.1f && vColor.g < 0.1f && vColor.b < 0.1f)
        vColor.a = 0.f;
    
    if (vColor.a < 0.1f)
        discard;
    
    return vColor;
}

float4 PS_MAIN_ALPHA(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    float4 vAlphaTexture = (float4) 0;
    
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
    vAlphaTexture = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);
    
    if (vAlphaTexture.a < 0.1f)
        discard;
    
    return vColor;
}

float4 PS_MAIN_FOCUS(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    float4 vAlphaTexture = (float4) 0;
    
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
    vAlphaTexture = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);
    
    if (vAlphaTexture.a < 0.1f)
        discard;
    
    return vColor;
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

    pass CoolTime // 6: 스킬 쿨타임
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_COOLTIME();
    }

    pass Color // 7: 소드트레일
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }

    pass Sprite // 8: 텍스처 재생
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_SPRITE();
    }

    pass Alpha // 9: 알파텍스처 블렌드
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
    }

    pass AlphaBlend // 10: 알파텍스처 테스트
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
    }

    pass Focus // 11: 포커스
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_FOCUS();
    }
};