#include "pch.h"
#include "CTerrain.h"

#include "CGameInstance.h"
#include "CToolInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject3D(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject3D(rhs)
	, m_fDetail(rhs.m_fDetail)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	m_fDetail = 1.f;
	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	TOOL->m_pTerrain = this;

	D3D11_RASTERIZER_DESC RasterizerDesc;
	ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

	RasterizerDesc.CullMode = { D3D11_CULL_BACK };
	RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };
	RasterizerDesc.FrontCounterClockwise = { false };

	m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRasterizer);

	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	if(nullptr != m_pRenderCom)
		m_pRenderCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

void CTerrain::Late_Tick(_double TimeDelta)
{
}

HRESULT CTerrain::Render()
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CTerrain::Reset_Data(const _uint& iXCount, const _uint& iZCount,
	const _float& fInterval, const _float& fDetail)
{
	if (iXCount < 1 || iZCount < 1 || fInterval < 0 || fDetail < 0)
		return E_FAIL;

	if (nullptr == m_pBufferCom)
		return E_FAIL;

	m_fDetail = fDetail;

	return m_pBufferCom->Initialize_Prototype(iXCount, iZCount, fInterval);
}

HRESULT CTerrain::Reset_Rasterizer(const D3D11_RASTERIZER_DESC& RasterizerDesc)
{
	Safe_Release(m_pRasterizer);

	m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRasterizer);
	return S_OK;
}

HRESULT CTerrain::Set_Position(const _float4& rPosition)
{
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&rPosition));

	return S_OK;
}

HRESULT CTerrain::Set_Rotation(const _float3& rDegrees)
{
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	_float3 Scales = m_pTransformCom->Get_Scaled();
	_matrix RotationMatrixX, RotationMatrixY, RotationMatrixZ;
	
	RotationMatrixX = XMMatrixRotationX(XMConvertToRadians(rDegrees.x));
	RotationMatrixY = XMMatrixRotationY(XMConvertToRadians(rDegrees.y));
	RotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(rDegrees.z));

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * Scales.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * Scales.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * Scales.z;

	_matrix RotationMatrix = RotationMatrixX * RotationMatrixY * RotationMatrixZ;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

	return S_OK;
}

HRESULT CTerrain::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_VIBuffer_Terrain",
		L"Com_Buffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Shader_VtxNorTex",
		L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Renderer",
		L"Com_Renderer", (CComponent**)&m_pRenderCom)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Texture_Test",
		L"Com_Texture_Test", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Set_Shader_Resources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Rasterizer("g_Rasterizer", 0, m_pRasterizer)))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_Float("g_fDetail", m_fDetail)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		AfxMessageBox(L"Failed to Created CTerrain");
	}
	return pInstance;
}

CTerrain* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		AfxMessageBox(L"Failed to Cloned CTerrain");
	}
	return pInstance;
}

void CTerrain::Free()
{
	Safe_Release(m_pRasterizer);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRenderCom);
	CGameObject3D::Free();
}
