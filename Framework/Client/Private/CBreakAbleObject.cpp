#include "CBreakAbleObject.h"

#include "CGameInstance.h"
#include "CSoul.h"

CBreakAbleObject::CBreakAbleObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CBreakAbleObject::CBreakAbleObject(const CBreakAbleObject& rhs)
	: CGameObject3D(rhs)
{
}

void CBreakAbleObject::Get_Damaged(const CAttack* pAttack)
{
	m_pHealth->Damaged(pAttack->Get_Damage());
}

HRESULT CBreakAbleObject::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	BREAKABLEDESC* pDesc = static_cast<BREAKABLEDESC*>(pArg);
	wstring wstrModelTag = pDesc->wstrModelTag;

	if (FAILED(Add_Component(iLevelIndex, wstrModelTag.c_str(), L"Com_Model",
		(CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	m_pTransformCom->Set_Matrix(pDesc->WorldMatrix);
	m_pTransformCom->Set_Angle(pDesc->vAngle);

	CBounding_AABB::AABBDESC Desc;
	Desc.vExtents = pDesc->vExtents;
	Desc.eGroup = CCollider::COL_STATIC;
	Desc.vPosition = _float3(0.f, 0.f, 0.f);
	Desc.vOffset = _float3(0.f, 0.f, 0.f);
	Desc.isEnable = true;

	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Com_Collider", &Desc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBreakAbleObject::Tick(const _double& TimeDelta)
{
	if (true == m_pHealth->isDead())
		Dead_Motion(TimeDelta);

	Tick_Colliders(m_pTransformCom->Get_WorldMatrix());
}

void CBreakAbleObject::AfterFrustumTick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f))
	{
		if (FAILED(Add_Colliders_To_Manager()))
		{
			MSG_BOX("Failed to Add Colliders To Manager");
			Safe_Release(pGameInstance);
			return;
		}

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

		for (auto& pSoul : m_vecSouls)
			pSoul->AfterFrustumTick(TimeDelta);

#ifdef _DEBUG
		if (true == m_isRender && FAILED(Add_Colliders_Debug_Render_Group(m_pRendererCom)))
			return;
#endif
	}

	Safe_Release(pGameInstance);
}

void CBreakAbleObject::Late_Tick(const _double& TimeDelta)
{
	On_Colisions(TimeDelta);
}

HRESULT CBreakAbleObject::Render()
{
	if (true == m_pHealth->isDead())
		return S_OK;

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

void CBreakAbleObject::Dead_Motion(const _double& TimeDelta)
{
	for (auto iter = m_vecSouls.begin(); iter !=  m_vecSouls.end();)
	{
		(*iter)->Tick(TimeDelta);
		if (true == (*iter)->is_Remove())
		{
			Safe_Release(*iter);
			iter = m_vecSouls.erase(iter);
		}
		else
			++iter;
	}

	if (0 == m_vecSouls.size())
		m_isRemove = true;
}

HRESULT CBreakAbleObject::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_Mesh", L"Com_Shader_Mesh",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	CHealth::HEALTHDESC Desc;
	Desc.iMaxHP = 1;
	Desc.iHP = 1;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Status_Health", L"Com_Health",
		(CComponent**)&m_pHealth, this, &Desc)))
		return E_FAIL;


	CSoul::SOULDESC tSoulDesc;
	XMStoreFloat4(&tSoulDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	tSoulDesc.RotationPerSec = XMConvertToRadians(90.f);
	tSoulDesc.SpeedPerSec = 2.f;

	for (_uint i = 0; i < 4; ++i)
	{
		CSoul* pSoul = { nullptr };
		wstring wstrTag = L"Soul" + to_wstring(i);
		if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Soul", wstrTag,
			(CComponent**)&pSoul, this, &tSoulDesc)))
			return E_FAIL;

		m_vecSouls.push_back(pSoul);
	}

	return S_OK;
}

HRESULT CBreakAbleObject::Bind_ShaderResources()
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

CBreakAbleObject* CBreakAbleObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBreakAbleObject* pInstance = new CBreakAbleObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBreakAbleObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBreakAbleObject* CBreakAbleObject::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CBreakAbleObject* pInstance = new CBreakAbleObject(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CBreakAbleObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBreakAbleObject::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pHealth);

	CGameObject3D::Free();
}
