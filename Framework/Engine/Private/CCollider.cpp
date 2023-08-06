#include "CCollider.h"

#include "CBounding_OBB.h"
#include "CBounding_AABB.h"
#include "CBounding_Sphere.h"

#include "CGameObject3D.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_eColliderType(rhs.m_eColliderType)
{
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	switch (eType)
	{
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext);
		m_eColliderType = TYPE_SPHERE;
		if (nullptr == m_pBounding)
			return E_FAIL;
		break;

	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext);
		m_eColliderType = TYPE_AABB;
		if (nullptr == m_pBounding)
			return E_FAIL;
		break;

	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext);
		m_eColliderType = TYPE_OBB;
		if (nullptr == m_pBounding)
			return E_FAIL;
		break;
	}

	return S_OK;
}

HRESULT CCollider::Initialize(const _uint& iLevelIndex, CComponent* pOwner, CBounding* pBounding, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		COLLIDERDESC Desc = *reinterpret_cast<COLLIDERDESC*>(pArg);
		m_eColGroup = Desc.eGroup;
		m_vOffset = Desc.vOffset;
		m_isEnable = Desc.isEnable;
	}

	m_pBounding = pBounding->Clone(pArg);
	if (nullptr == m_pBounding)
		return E_FAIL;

	return S_OK;
}

void CCollider::Tick(_fmatrix WorldMatrix)
{
	if (nullptr == m_pBounding)
		return;

	m_pBounding->Tick(m_vOffset, WorldMatrix);
}

HRESULT CCollider::Render()
{
	if (nullptr == m_pBounding)
		return E_FAIL;

	m_pBounding->Render();

	return S_OK;
}

_bool CCollider::Intersect(const CCollider* pCollider)
{
	if (false == m_isEnable)
		return false;

	return m_pBounding->Intersect(pCollider->m_eColliderType, pCollider->m_pBounding);
}

void CCollider::On_Collision(CGameObject3D* pOnwer, const _double& TimeDelta)
{
	/* 충돌체크 검사 전에 리스트의 모든 콜리전을 충돌하지 않은상태로 초기화 */
	for (auto& Pair : m_umapCollisions)
		Pair.second->isCollision = false;

	/* 메시지 검사 */
	while (true != m_Qmessage.empty())
	{
		CCollider* pOther = m_Qmessage.front();

		/* 충돌체크를 하면서 콜리전의 상태를 충돌한 상태로 변경 */
		COLLISION* pCollision = { nullptr };
		Find_Collision(pOther, &pCollision);

		switch (pCollision->eState)
		{
		case STATE_NONE:
			pCollision->eState = STATE_ENTER;
			pOnwer->OnCollisionEnter(*pCollision, TimeDelta);
			break;

		case STATE_ENTER:
			pCollision->eState = STATE_STAY;
			pOnwer->OnCollisionStay(*pCollision, TimeDelta);
			break;

		case STATE_STAY:
			pOnwer->OnCollisionStay(*pCollision, TimeDelta);
			break;
		}

		m_Qmessage.pop();
	}

	/* 전체 메시지를 다 확인했는데 리스트에 있는 콜라이더의 상태가 충돌인 상태가 아닌경우 리스트에서 삭제 */
	for (auto iter = m_umapCollisions.begin(); iter != m_umapCollisions.end();)
	{
		if (false == iter->second->isCollision)
		{
			pOnwer->OnCollisionExit(*iter->second, TimeDelta);

			Safe_Release(iter->second->pOther);
			Safe_Release(iter->second->pOtherCollider);

			Safe_Delete(iter->second);

			iter = m_umapCollisions.erase(iter);
		}
		else
			++iter;
	}
}

void CCollider::Find_Collision(CCollider* pCollider, COLLISION** pCollision)
{
	auto iter = find_if(m_umapCollisions.begin(), m_umapCollisions.end(), [&](auto Pair)
		{
			if (Pair.first == pCollider)
				return true;
			else
				return false;
		});

	if (iter == m_umapCollisions.end())
	{
		*pCollision = new COLLISION;
		(*pCollision)->pOtherCollider = pCollider;
		Safe_AddRef(pCollider);
		(*pCollision)->pOther = static_cast<CGameObject3D*>(pCollider->m_pOwner);
		Safe_AddRef(pCollider->m_pOwner);
		(*pCollision)->pMyCollider = this;
		(*pCollision)->eState = STATE_NONE;
		(*pCollision)->isCollision = true;

		m_umapCollisions.emplace(pCollider, (*pCollision));

		return;
	}

	iter->second->isCollision = true;

	(*pCollision) = iter->second;
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCollider* CCollider::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, m_pBounding, pArg)))
	{
		MSG_BOX("Failed to Cloned CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	Safe_Release(m_pBounding);

	for (auto& Pair : m_umapCollisions)
	{
		Safe_Release(Pair.second->pOther);
		Safe_Release(Pair.second->pOtherCollider);

		Safe_Delete(Pair.second);
	}

	CComponent::Free();
}

const _float3& CCollider::Get_Extents() const
{
	if (m_eColliderType == TYPE_AABB)
		return static_cast<CBounding_AABB*>(m_pBounding)->Get_Extents();

	return _float3();
}

void CCollider::Set_Extents(const _float3& vExtents)
{
	if (m_eColliderType == TYPE_AABB)
		static_cast<CBounding_AABB*>(m_pBounding)->Set_Extents(vExtents);
}
