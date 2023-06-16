
RasterizerState g_Rasterizer;
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
};

struct VS_IN
{
	float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;
	
    matrix ViewProjMatrix = mul(g_ViewMatrix, g_ProjMatrix);
    matrix WVPMatrix = mul(g_WorldMatrix, ViewProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), WVPMatrix);

	return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
};

float4 PS_MAIN(PS_IN In) : SV_TARGET0
{
    float4 vColor = In.vPosition;
	
	return vColor;
}

technique11 DefaultTechnique
{
	pass Coordnate_Axis
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
        SetRasterizerState(g_Rasterizer);
    }
};
