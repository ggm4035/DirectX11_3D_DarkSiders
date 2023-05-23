
 float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
 float4x4 g_WorldViewProjMatrix;

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
	VS_OUT Out = (VS_OUT)0;
	
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldViewProjMatrix);
  
    Out.vTexUV = In.vTexUV;
	
	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

float4 PS_MAIN(PS_IN In) : SV_TARGET
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);

	return vColor;
}

RasterizerState WireframeRS
{
    FillMode = Wireframe;
    CullMode = None;
    FrontCounterClockwise = false;
};

technique11 DefaultTechnique
{
	pass Terrain
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();

        SetRasterizerState(WireframeRS);
    }
};