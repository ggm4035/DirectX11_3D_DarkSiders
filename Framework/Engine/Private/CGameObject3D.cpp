#include "CGameObject3D.h"

#include "CGameInstance.h"
#include "CCollider_Manager.h"

CGameObject3D::CGameObject3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CGameObject3D::CGameObject3D(const CGameObject3D& rhs)
	: CGameObject(rhs)
	, m_Status(rhs.m_Status)
{
}

void CGameObject3D::Get_Damaged()
{
	m_eCurHitState = HIT;
	m_Status.iHP -= 1;

	if (m_Status.iHP <= 0)
		m_isDead = true;
}

HRESULT CGameObject3D::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComposite::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Clone_Transform(this, pArg));
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CGameObject3D::Tick(const _double& TimeDelta)
{
	CGameObject::Tick(TimeDelta);
}

void CGameObject3D::Late_Tick(const _double& TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
}

CCollider* CGameObject3D::Find_Collider(const wstring& wstrColliderTag)
{
	auto iter = m_umapColliders.find(wstrColliderTag);

	if (iter == m_umapColliders.end())
		return nullptr;

	return iter->second;
}

HRESULT CGameObject3D::Add_Collider(const _uint& iLevelIndex, const wstring& wstrPrototypeTag, const wstring& wstrColliderTag, void* pArg)
{
	if (nullptr != Find_Collider(wstrColliderTag))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCollider* pCollider = dynamic_cast<CCollider*>(pGameInstance->Clone_Component(iLevelIndex, wstrPrototypeTag, this, pArg));
	if (nullptr == pCollider)
		return E_FAIL;

	pCollider->Set_Tag(wstrColliderTag);

	Safe_Release(pGameInstance);
	m_umapColliders.emplace(wstrColliderTag, pCollider);

	return S_OK;
}

HRESULT CGameObject3D::Add_Colliders_To_Manager()
{
	CCollider_Manager* pCollider_Manager = CCollider_Manager::GetInstance();
	Safe_AddRef(pCollider_Manager);

	for (auto Pair : m_umapColliders)
	{
		if (FAILED(pCollider_Manager->Add_Collider(Pair.second)))
			return E_FAIL;
	}

	Safe_Release(pCollider_Manager);

	return S_OK;
}

void CGameObject3D::Tick_Colliders(_fmatrix WorldMatrix)
{
	for (auto Pair : m_umapColliders)
		Pair.second->Tick(WorldMatrix);
}

void CGameObject3D::On_Colisions(const _double& TimeDelta)
{
	for (auto Pair : m_umapColliders)
		Pair.second->On_Collision(this, TimeDelta);
}

HRESULT CGameObject3D::Add_Colliders_Debug_Render_Group(CRenderer* pRenderer)
{
	for (auto& Pair : m_umapColliders)
	{
		if (FAILED(pRenderer->Add_DebugGroup(Pair.second)))
			return E_FAIL;
	}
	return S_OK;
}
#ifdef _DEBUG
#endif // _DEBUG

void CGameObject3D::Free()
{
	for (auto& Pair : m_umapColliders)
		Safe_Release(Pair.second);
	m_umapColliders.clear();

	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
