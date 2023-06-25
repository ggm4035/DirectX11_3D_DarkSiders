#include "CBounding_Sphere.h"

#include "CCollider.h"
#include "CBounding_AABB.h"
#include "CBounding_OBB.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

CBounding_Sphere::CBounding_Sphere(const CBounding_Sphere& rhs)
	: CBounding(rhs)
	, m_pSphere_Original(new BoundingSphere(*rhs.m_pSphere_Original))
	, m_pSphere(new BoundingSphere(*rhs.m_pSphere_Original))
{
}

HRESULT CBounding_Sphere::Initialize_Prototype()
{
	if (FAILED(CBounding::Initialize_Prototype()))
		return E_FAIL;

	m_pSphere_Original = new BoundingSphere;
	m_pSphere = new BoundingSphere;

	m_pSphere_Original->Center = _float3(0.f, 0.f, 0.f);
	m_pSphere_Original->Radius = 0.5f;


	return S_OK;
}

HRESULT CBounding_Sphere::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_pSphere_Original->Center = reinterpret_cast<SPHEREDESC*>(pArg)->vPosition;
	m_pSphere_Original->Radius = reinterpret_cast<SPHEREDESC*>(pArg)->fRadius;

	*m_pSphere = *m_pSphere_Original;

	return S_OK;
}

void CBounding_Sphere::Tick(_fmatrix WorldMatrix)
{
	m_pSphere_Original->Transform(*m_pSphere, WorldMatrix);
}

HRESULT CBounding_Sphere::Render()
{
	Begin_Batch();

	DX::Draw(m_pBatch, *m_pSphere, true == m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	End_Batch();

	return S_OK;
}

_bool CBounding_Sphere::Intersect(CCollider::TYPE eType, const CBounding* pBounding)
{
	m_isColl = { false };

	switch (eType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		m_isColl = m_pSphere->Intersects(*((CBounding_Sphere*)pBounding)->m_pSphere);
		break;

	case Engine::CCollider::TYPE_AABB:
		m_isColl = m_pSphere->Intersects(*((CBounding_Sphere*)pBounding)->m_pSphere);
		break;

	case Engine::CCollider::TYPE_OBB:
		m_isColl = m_pSphere->Intersects(*((CBounding_Sphere*)pBounding)->m_pSphere);
		break;
	}

	return m_isColl;
}

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding_Sphere* CBounding_Sphere::Clone(void* pArg)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pSphere);

	CBounding::Free();
}
