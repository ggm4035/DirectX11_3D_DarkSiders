matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_Weights[23] =
{
    0.0011, 0.0123, 0.0561, 0.1353, 0.278, 0.3001, 0.4868, 0.6666, 0.7261, 0.8712, 0.9231,
    0.9986, 0.9231, 0.8712, 0.7261, 0.6666, 0.4868, 0.3001, 0.278, 0.1353, 0.0561, 0.0123, 0.0011
};

float g_fTexWidth, g_fTexHeight;
float g_fFocusPower, g_fFocusDetail;

texture2D g_Texture;
texture2D g_BlurTexture;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

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

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vPostProcessingTex = g_Texture.Sample(LinearSampler, In.vTexUV);
    vector vBlurTex = g_BlurTexture.Sample(LinearSampler, In.vTexUV);
    
    Out.vColor = vPostProcessingTex + vBlurTex;

    return Out;
}

PS_OUT PS_MAIN_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vTexUV = 0.f;
    float fTexU = 1.f / g_fTexWidth;
    float fTotal = 0.f;
    
    vector vBlurTex = g_Texture.Sample(LinearSampler, In.vTexUV);
    
    for (int i = -11; i < 11; ++i)
    {
        vTexUV = In.vTexUV + float2(fTexU * i, 0.f);
        Out.vColor += g_Weights[11 + i] * g_Texture.Sample(LinearSampler, vTexUV);
        fTotal += g_Weights[11 + i];
    }
    
    Out.vColor /= fTotal;

    return Out;
}

PS_OUT PS_MAIN_ZOOMBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBlurTex = g_BlurTexture.Sample(LinearSampler, In.vTexUV);
    float2 vTexUV = 0.f;
    
    float2 fTexelSize = float2(1.f / g_fTexWidth, 1.f / g_fTexHeight);
    float2 vFocus = In.vTexUV - float2(0.5f, 0.5f);
    
    for (float i = 0.f; i < g_fFocusDetail; i += 1.f)
    {
        float fScale = 1.f - g_fFocusPower * fTexelSize.x * i;
        vTexUV = vFocus * fScale + float2(0.5f, 0.5f);
        Out.vColor += g_Texture.Sample(LinearSampler, vTexUV);
    }
    
    Out.vColor *= 1.f / g_fFocusDetail;
    Out.vColor.a = 1.f;

    Out.vColor += vBlurTex;
    
    return Out;
}

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_Depth_Disable
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

technique11 DefaultTechnique
{
    pass Default // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass OneDimBlur // 1 1차원 블러
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR();
    }

    pass ZoomBlur // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_ZOOMBLUR();
    }
}