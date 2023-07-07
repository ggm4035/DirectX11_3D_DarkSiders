#include "CBounding_OBB.h"

#include "CBounding_AABB.h"
#include "CBounding_Sphere.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

CBounding_OBB::CBounding_OBB(const CBounding_OBB& rhs)
	: CBounding(rhs)
	, m_pOBB_Original(new BoundingOrientedBox(*rhs.m_pOBB_Original))
	, m_pOBB(new BoundingOrientedBox(*rhs.m_pOBB_Original))
{
}

HRESULT CBounding_OBB::Initialize_Prototype()
{
	if (FAILED(CBounding::Initialize_Prototype()))
		return E_FAIL;

	m_pOBB_Original = new BoundingOrientedBox;
	m_pOBB = new BoundingOrientedBox;

	m_pOBB_Original->Center = _float3(0.f, 0.f, 0.f);
	m_pOBB_Original->Extents = _float3(0.5f, 0.5f, 0.5f);
	m_pOBB_Original->Orientation = _float4(0.f, 0.f, 0.f, 1.f);

	return S_OK;
}

HRESULT CBounding_OBB::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_pOBB_Original->Center = reinterpret_cast<OBBDESC*>(pArg)->vPosition;
	m_pOBB_Original->Extents = reinterpret_cast<OBBDESC*>(pArg)->vExtents;
	XMStoreFloat4(&m_pOBB_Original->Orientation, 
		XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&reinterpret_cast<OBBDESC*>(pArg)->vRotation)));

	*m_pOBB = *m_pOBB_Original;

	return S_OK;
}

void CBounding_OBB::Tick(_fmatrix WorldMatrix)
{
	m_pOBB_Original->Transform(*m_pOBB, WorldMatrix);
}

HRESULT CBounding_OBB::Render()
{
	Begin_Batch();

	DX::Draw(m_pBatch, *m_pOBB, true == m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	End_Batch();

	return S_OK;
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eType, CBounding* pBounding)
{
	m_isColl = { false };

	switch (eType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		m_isColl = m_pOBB->Intersects(*dynamic_cast<CBounding_Sphere*>(pBounding)->Get_BoundingSphere());
		break;

	case Engine::CCollider::TYPE_AABB:
		m_isColl = m_pOBB->Intersects(*dynamic_cast<CBounding_AABB*>(pBounding)->Get_BoundingAABB());
		break;

	case Engine::CCollider::TYPE_OBB:
		m_isColl = m_pOBB->Intersects(*dynamic_cast<CBounding_OBB*>(pBounding)->Get_BoundingOBB());
		break;
	}

	return m_isColl;
}

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding_OBB* CBounding_OBB::Clone(void* pArg)
{
	CBounding_OBB* pInstance = new CBounding_OBB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pOBB);

	CBounding::Free();
}
