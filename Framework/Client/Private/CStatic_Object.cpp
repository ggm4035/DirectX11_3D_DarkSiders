#include "CStatic_Object.h"

#include "CGameInstance.h"

CStatic_Object::CStatic_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CStatic_Object::CStatic_Object(const CStatic_Object& rhs)
	:CGameObject3D(rhs)
{
}

HRESULT CStatic_Object::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStatic_Object::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

void CStatic_Object::Tick(const _double& TimeDelta)
{
}

void CStatic_Object::AfterFrustumTick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}

	Safe_Release(pGameInstance);
}

void CStatic_Object::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CStatic_Object::Render()
{
	if (m_wstrTag == L"Rocks_Circle")
		int i = 0;

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

HRESULT CStatic_Object::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_Mesh", L"Com_Shader_Mesh",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatic_Object::Bind_ShaderResources()
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

CStatic_Object* CStatic_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStatic_Object* pInstance = new CStatic_Object(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CStatic_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CStatic_Object* CStatic_Object::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CStatic_Object* pInstance = new CStatic_Object(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CStatic_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStatic_Object::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	CGameObject3D::Free();
}
