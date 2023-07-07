#include "CCollider.h"

#include "CBounding_OBB.h"
#include "CBounding_AABB.h"
#include "CBounding_Sphere.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
{
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	switch (eType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext);
		if (nullptr == m_pBounding)
			return E_FAIL;

		break;

	case Engine::CCollider::TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext);
		if (nullptr == m_pBounding)
			return E_FAIL;

		break;

	case Engine::CCollider::TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext);
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

	m_pBounding = pBounding->Clone(pArg);
	if (nullptr == m_pBounding)
		return E_FAIL;

	return S_OK;
}

void CCollider::Tick(_fmatrix WorldMatrix)
{
	if (nullptr == m_pBounding)
		return;

	m_pBounding->Tick(WorldMatrix);
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
	return m_pBounding->Intersect(pCollider->m_eColliderType, pCollider->m_pBounding);
}

void CCollider::On_Collision()
{
	while (true != m_Qmessage.empty())
	{
		CCollider* pOther = m_Qmessage.front();



		m_Qmessage.pop();
	}



	switch (m_eCurrentState)
	{
	case Engine::CCollider::STATE_NONE:
		m_eCurrentState = STATE_ENTER;
		break;

	case Engine::CCollider::STATE_ENTER:
		m_eCurrentState = STATE_STAY;
		break;

	case Engine::CCollider::STATE_STAY:

		break;
	}
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

	CComponent::Free();
}
