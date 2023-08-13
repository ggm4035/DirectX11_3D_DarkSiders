#include "CObstacle.h"

#include "CGameInstance.h"

CObstacle::CObstacle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CObstacle::CObstacle(const CObstacle& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CObstacle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CObstacle::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	STATICOBJECTDESC* pDesc = reinterpret_cast<STATICOBJECTDESC*>(pArg);
	wstring wstrModelTag = pDesc->wstrModelTag;

	if (FAILED(Add_Component(iLevelIndex, wstrModelTag.c_str(), L"Com_Model",
		(CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	m_pTransformCom->Set_Matrix(pDesc->WorldMatrix);
	m_pTransformCom->Set_Angle(pDesc->vAngle);

	return S_OK;
}

void CObstacle::Tick(const _double& TimeDelta)
{
}

void CObstacle::AfterFrustumTick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	}

	Safe_Release(pGameInstance);
}

void CObstacle::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CObstacle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TYPE_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TYPE_NORMALS);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CObstacle::Render_Shadow()
{
	if (FAILED(Set_Shader_Shadow_Resources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CObstacle::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_Mesh", L"Com_Shader_Mesh",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObstacle::Bind_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 InputMatrix = m_pTransformCom->Get_WorldFloat4x4();
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &InputMatrix)))
		return E_FAIL;

	InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &InputMatrix)))
		return E_FAIL;

	InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &InputMatrix)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CObstacle::Set_Shader_Shadow_Resources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 InputFloat4x4 = m_pTransformCom->Get_WorldFloat4x4();
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &InputFloat4x4)))
		return E_FAIL;

	InputFloat4x4 = pGameInstance->Get_LightViewFloat4x4(0);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_LightViewMatrix", &InputFloat4x4)))
		return E_FAIL;

	InputFloat4x4 = pGameInstance->Get_LightProjFloat4x4(m_pContext);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_LightProjMatrix", &InputFloat4x4)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CObstacle* CObstacle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CObstacle* pInstance = new CObstacle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CObstacle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CObstacle* CObstacle::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CObstacle* pInstance = new CObstacle(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CObstacle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObstacle::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	CGameObject3D::Free();
}
