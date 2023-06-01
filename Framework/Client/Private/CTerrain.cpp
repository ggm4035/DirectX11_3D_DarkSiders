#include "stdafx.h"
#include "CTerrain.h"

#include "CGameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject3D(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	:CGameObject3D(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(CComponent* pOwner, void* pArg)
{
 	if (FAILED(CGameObject3D::Initialize(pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Tick(const _double& TimeDelta)
{
}

void CTerrain::Late_Tick(const _double& TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CTerrain::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_VIBuffer_Terrain",
		L"Com_Buffer_Terrain", (CComponent**)&m_pBufferCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Texture_Terrain",
		L"Com_Texture_Terrain", (CComponent**)&m_pTextureCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxNorTex",
		L"Com_Shader_VtxNorTex", (CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Renderer",
		L"Com_Renderer", (CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Diffuse_Texture")))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject3D* CTerrain::Clone(CComponent* pOwner, void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	CGameObject3D::Free();
}
