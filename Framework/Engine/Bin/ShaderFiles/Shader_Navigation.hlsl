
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector			g_vColor;

struct VS_IN
{
	float3		vPosition : POSITION;	
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;	
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
float4	PS_MAIN(PS_IN In) : SV_TARGET0
{
	float4		vColor = (float4)0;

	vColor = g_vColor;

	return vColor;
}

technique11		DefaultTechnique
{
	pass Cell
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	
}