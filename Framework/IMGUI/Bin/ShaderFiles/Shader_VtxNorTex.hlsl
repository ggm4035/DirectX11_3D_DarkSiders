
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

RasterizerState g_Rasterizer;

float4 g_LightPosition, g_LightDirection;
float g_LightRange;
float4 g_LightDiffuse, g_LightSpecular, g_LightAmbient;

float4 g_CameraPosition;

texture2D g_DiffuseTexture;

texture2D g_BrushTexture;
float4 g_vBrushPos;
float g_fBrushRadius;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
    float4 vNoraml : NORAML;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPosition : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;
	
    matrix ViewProjMatrix = mul(g_ViewMatrix, g_ProjMatrix);
    matrix WVPMatrix = mul(g_WorldMatrix, ViewProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), WVPMatrix);
    Out.vNoraml = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORAML;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

float4 PS_MAIN(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    float4 vBrush = (float4) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 30.f);
    
    if (g_vBrushPos.x > 0 &&
        g_vBrushPos.x - g_fBrushRadius < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRadius &&
		g_vBrushPos.z - g_fBrushRadius < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRadius)
    {
        float2 vTexUV;
        
        vTexUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRadius)) / (2.f * g_fBrushRadius);
        vTexUV.y = ((g_vBrushPos.z - g_fBrushRadius) - In.vWorldPos.z) / (2.f * g_fBrushRadius);

        vBrush = g_BrushTexture.Sample(LinearSampler, vTexUV);
        vDiffuse += vBrush * 0.2f;
    }

    float4 vAmbient = float4(0.4f, 0.4f, 0.4f, 1.f);
    float fShade = max(dot(-normalize(g_LightDirection), In.vNormal), 0.f);
    fShade = saturate(fShade + vAmbient * g_LightAmbient);
    
    float4 vReflect = normalize(reflect(normalize(g_LightDirection), normalize(In.vNormal)));
    float4 vLook = normalize(In.vWorldPos - g_CameraPosition);
    
    vDiffuse = vDiffuse * g_LightDiffuse * fShade;
    float4 vSpecular = pow(max(dot(-vReflect, vLook), 0.f), 30.f) * g_LightSpecular;
    
    vColor = vDiffuse + vSpecular;
    
    return vColor;
}

technique11 DefaultTechnique
{
	pass Terrain
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
        SetRasterizerState(g_Rasterizer);
    }
};
