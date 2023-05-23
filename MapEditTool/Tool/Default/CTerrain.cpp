#include "pch.h"
#include "CTerrain.h"

#include "CVIBuffer_Terrain.h"
#include "CGameInstance.h"

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

	_matrix WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = XMMatrixIdentity();//XMMatrixInverse(nullptr, *pGameInstance->Get_Current_CameraViewMatrix());

	ViewMatrix = *pGameInstance->Get_Current_CameraViewMatrix();
	ProjMatrix = *pGameInstance->Get_Current_CameraProjMatrix();

	WVPMatrix = WorldMatrix * ViewMatrix * ProjMatrix;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	//m_pShaderCom->Set_Matrix("g_ViewMatrix", &ViewMatrix);
	//m_pShaderCom->Set_Matrix("g_ProjMatrix", &ProjMatrix);
	if(FAILED(m_pShaderCom->Set_Matrix("g_WorldViewProjMatrix", &WVPMatrix)))
		return E_FAIL;

	Safe_Release(pGameInstance);
	m_pShaderCom->Begin(0);

	m_pBufferCom->Render();

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
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRenderCom);
	CGameObject::Free();
}
