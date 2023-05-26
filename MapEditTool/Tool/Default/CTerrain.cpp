#include "pch.h"
#include "CTerrain.h"

#include "CVIBuffer_Terrain.h"
#include "CGameInstance.h"
#include "CToolInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
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
	CGameObject::Tick(TimeDelta);

	m_pRenderCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

void CTerrain::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
}

HRESULT CTerrain::Render()
{
	if (FAILED(CGameObject::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_matrix WorldViewProjMatrix, WorldMatrix, ViewMatrix, ProjMatrix;
	
	WorldMatrix = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix());
	ViewMatrix = pGameInstance->Get_Current_CameraViewMatrix();
	ProjMatrix = pGameInstance->Get_Current_CameraProjMatrix();

	WorldViewProjMatrix = WorldMatrix * ViewMatrix * ProjMatrix;

	_float4x4 WVP;
	XMStoreFloat4x4(&WVP, WorldViewProjMatrix);

	if (FAILED(m_pShaderCom->Bind_Rasterizer("g_Rasterizer", 0, m_pRasterizer)))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_Float4x4("g_WorldViewProjMatrix", &WVP)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	Safe_Release(pGameInstance);
	m_pShaderCom->Begin(0);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CTerrain::Reset_Data(_uint iXCount, _uint iZCount, _float fInterval)
{
	if (nullptr == m_pBufferCom)
		return E_FAIL;

	return m_pBufferCom->Initialize_Prototype(iXCount, iZCount, fInterval);
}

HRESULT CTerrain::Reset_Rasterizer(const D3D11_RASTERIZER_DESC& RasterizerDesc)
{
	Safe_Release(m_pRasterizer);

	m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRasterizer);
	return S_OK;
}

HRESULT CTerrain::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_VIBuffer_Terrain",
		L"Com_Buffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Shader_Vtxtex",
		L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Renderer",
		L"Com_Renderer", (CComponent**)&m_pRenderCom)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Texture_Test",
		L"Com_Texture_Test", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Transform",
		L"Com_Transform", (CComponent**)&m_pTransformCom, &CTransform::TRASNFORMDESC(10.f, XMConvertToRadians(90.f)))))
		return E_FAIL;

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

CGameObject* CTerrain::Clone(void* pArg)
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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRenderCom);
	CGameObject::Free();
}
