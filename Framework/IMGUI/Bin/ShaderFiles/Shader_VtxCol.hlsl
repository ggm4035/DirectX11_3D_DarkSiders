#include "Shader_Imgui_Defines.hlsli"

RasterizerState g_Rasterizer;
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

struct VS_IN
{
	float3 vPosition : POSITION;
	float4 vColor : COLOR;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix ViewProjMatrix = mul(g_ViewMatrix, g_ProjMatrix);
    matrix WVPMatrix = mul(g_WorldMatrix, ViewProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), WVPMatrix);
    Out.vColor = In.vColor;

    return Out;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

float4 PS_MAIN(PS_IN In) : SV_TARGET0
{
    float4 vColor = In.vColor;
	
    vColor.r = vColor.r > 0.0001f ? 1.f : 0.f;
    vColor.g = vColor.g > 0.0001f ? 1.f : 0.f;
    vColor.b = vColor.b > 0.0001f ? 1.f : 0.f;
    
    return vColor;
};

technique11 DefaultTechnique
{
	pass Coordnate_Axis
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
        SetRasterizerState(g_Rasterizer);
    }
};
