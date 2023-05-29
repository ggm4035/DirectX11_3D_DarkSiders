#include "pch.h"
#include "CCoordnate_Axis.h"

#include "CGameInstance.h"
#include "CToolInstance.h"

CCoordnate_Axis::CCoordnate_Axis(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CCoordnate_Axis::CCoordnate_Axis(const CCoordnate_Axis& rhs)
	: CGameObject3D(rhs)
	, m_bIsRender(rhs.m_bIsRender)
{
}

HRESULT CCoordnate_Axis::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCoordnate_Axis::Initialize(void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	TOOL->m_pCoordnate_Axis = this;

	return S_OK;
}

void CCoordnate_Axis::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	_vector vCameraPosition = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::STATE_VIEW)).r[3];

	_vector vDistance = XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vCameraPosition);
	vDistance /= 5.f;
	_float3 fScales;
	XMStoreFloat3(&fScales, vDistance);

	m_pTransformCom->Scaled(fScales);

	Safe_Release(pGameInstance);
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCoordnate_Axis::Render()
{
	if (false == m_bIsRender)
		return S_OK;

	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CCoordnate_Axis::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Renderer",
		L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Shader_VtxCol",
		L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_VIBuffer_Coordnate",
		L"Com_Buffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCoordnate_Axis::Set_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CCoordnate_Axis* CCoordnate_Axis::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCoordnate_Axis* pInstance = new CCoordnate_Axis(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		AfxMessageBox(L"Failed to Created CCoordnate_Axis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCoordnate_Axis* CCoordnate_Axis::Clone(void* pArg)
{
	CCoordnate_Axis* pInstance = new CCoordnate_Axis(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		AfxMessageBox(L"Failed to Cloned CCoordnate_Axis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCoordnate_Axis::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShaderCom);
	CGameObject3D::Free();
}
