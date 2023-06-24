#include "CCollider.h"

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
		break;
	}

	return S_OK;
}

HRESULT CCollider::Initialize(const _uint& iLayerIndex, CComponent* pOwner, CBounding* pBounding, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLayerIndex, pOwner, pArg)))
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

CCollider* CCollider::Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(iLayerIndex, pOwner, m_pBounding, pArg)))
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
