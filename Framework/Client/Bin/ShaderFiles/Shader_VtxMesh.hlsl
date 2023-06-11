
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4 g_LightDirection;
float4 g_LightDiffuse, g_LightSpecular, g_LightAmbient;

float4 g_CameraPosition;

texture2D g_DiffuseTexture, g_NormalTexture;

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
    float3 vTangent : TANGENT;
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
    float4 vNoraml : NORAML;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPosition : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    float4 vAmbient = float4(0.2f, 0.2f, 0.2f, 0.2f);
    float fShade = max(dot(normalize(In.vNoraml), -normalize(g_LightDirection)), 0.f);
    fShade = saturate(fShade + g_LightAmbient * vAmbient);
    
    float4 vReflect = normalize(reflect(g_LightDirection, In.vNoraml));
    float4 vLook = normalize(In.vWorldPosition - g_CameraPosition);
    
    vDiffuse = vDiffuse * g_LightDiffuse * fShade;
    float4 vSpecular = pow(max(dot(-vLook, vReflect), 0.f), 40.f) * g_LightSpecular;
    
    Out.vColor = vDiffuse + vSpecular;
	
    return Out;
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
    }
};