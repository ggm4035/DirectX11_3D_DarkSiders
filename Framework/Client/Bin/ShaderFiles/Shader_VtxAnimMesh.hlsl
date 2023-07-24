

#include "Shader_Client_Defines.hlsli"


matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_BoneMatrices[256]; /* 이 메시를 그리기위해 사용되는 뼈들의 행려 VTF */
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
float g_fTimeAcc = 0.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};


struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};


/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(vector(cross(Out.vTangent.xyz, Out.vNormal.xyz), 0.f));

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    float3 vNormal = /*vNormalDesc.xyz;*/vNormalDesc.xyz * 2.f - 1.f; // 0 ~ 1 -> -1 ~ 1
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse = vDiffuse;

	/* Out.vNormal unorm : 0 ~ 1 */
	/* In.vNormal.xyz : -1 ~ 1 */
    Out.vNormal = vector(vNormal, 0.f); /*vector(vNormal * 0.5f + 0.5f, 0.f);*/
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
	
    return Out;
}

PS_OUT PS_MAIN_HIT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    float3 vNormal = /*vNormalDesc.xyz;*/vNormalDesc.xyz * 2.f - 1.f; // 0 ~ 1 -> -1 ~ 1
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    float fData = saturate(1.f - g_fTimeAcc);
    float3 vHitColor = float3(1.f, 0.f, 0.f) * fData;

    vDiffuse.xyz += vHitColor;
    Out.vDiffuse = vDiffuse;

	/* Out.vNormal unorm : 0 ~ 1*/
	/* In.vNormal.xyz : -1 ~ 1 */
    Out.vNormal = vector(vNormal, 0.f); /*vector(vNormal * 0.5f + 0.5f, 0.f);*/
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

    return Out;
}

technique11 DefaultTechnique
{
    pass AnimMesh
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

    pass Hit
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_HIT();
    }
}