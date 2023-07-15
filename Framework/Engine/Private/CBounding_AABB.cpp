#include "CBounding_AABB.h"

#include "CBounding_OBB.h"
#include "CBounding_Sphere.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

CBounding_AABB::CBounding_AABB(const CBounding_AABB& rhs)
	: CBounding(rhs)
	, m_pAABB_Original(new BoundingBox(*rhs.m_pAABB_Original))
	, m_pAABB(new BoundingBox(*rhs.m_pAABB_Original))
{
}

HRESULT CBounding_AABB::Initialize_Prototype()
{
	if (FAILED(CBounding::Initialize_Prototype()))
		return E_FAIL;

	m_pAABB_Original = new BoundingBox;
	m_pAABB = new BoundingBox;

	m_pAABB_Original->Center = _float3(0.f, 0.f, 0.f);
	m_pAABB_Original->Extents = _float3(0.5f, 0.5f, 0.5f);
	
	return S_OK;
}

HRESULT CBounding_AABB::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_pAABB_Original->Center = reinterpret_cast<AABBDESC*>(pArg)->vPosition;
	m_pAABB_Original->Extents = reinterpret_cast<AABBDESC*>(pArg)->vExtents;

	*m_pAABB = *m_pAABB_Original;

	return S_OK;
}

void CBounding_AABB::Tick(const _float3& vOffset, _fmatrix ParentWorldMatrix)
{
	m_isColl = { false };
	_matrix Matrix = XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z) * ParentWorldMatrix;
	m_pAABB_Original->Transform(*m_pAABB, Remove_Rotation(Matrix));
}

HRESULT CBounding_AABB::Render()
{
	Begin_Batch();

	DX::Draw(m_pBatch, *m_pAABB, true == m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	End_Batch();

	return S_OK;
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eType, CBounding* pBounding)
{

	switch (eType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		m_isColl = m_pAABB->Intersects(*dynamic_cast<CBounding_Sphere*>(pBounding)->Get_BoundingSphere());
		break;

	case Engine::CCollider::TYPE_AABB:
		m_isColl = m_pAABB->Intersects(*dynamic_cast<CBounding_AABB*>(pBounding)->Get_BoundingAABB());
		break;

	case Engine::CCollider::TYPE_OBB:
		m_isColl = m_pAABB->Intersects(*dynamic_cast<CBounding_OBB*>(pBounding)->Get_BoundingOBB());
		break;
	}

	return m_isColl;
}

_matrix CBounding_AABB::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		ResultMatrix = TransformMatrix;

	ResultMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	ResultMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	ResultMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	return ResultMatrix;
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding_AABB* CBounding_AABB::Clone(void* pArg)
{
	CBounding_AABB* pInstance = new CBounding_AABB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pAABB);

	CBounding::Free();
}
