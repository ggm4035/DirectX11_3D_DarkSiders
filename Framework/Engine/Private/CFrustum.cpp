#include "CFrustum.h"

#include "CPipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

HRESULT CFrustum::Initialize()
{
	m_vOriginal_Points[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginal_Points[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginal_Points[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginal_Points[3] = _float3(-1.f, -1.f, 0.f);
	m_vOriginal_Points[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginal_Points[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginal_Points[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginal_Points[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Tick()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_vector vPoints[8];

	for (_uint i = 0; i < 8; ++i)
	{
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vOriginal_Points[i]), pPipeLine->Get_Transform_Inverse_Matrix(CPipeLine::STATE_PROJ));
		vPoints[i] = XMVector3TransformCoord(vPoints[i], pPipeLine->Get_Transform_Inverse_Matrix(CPipeLine::STATE_VIEW));
	}

	XMStoreFloat4(&m_vWorldPlanes[0], XMPlaneFromPoints(vPoints[1], vPoints[5], vPoints[6]));
	XMStoreFloat4(&m_vWorldPlanes[1], XMPlaneFromPoints(vPoints[4], vPoints[0], vPoints[3]));
	XMStoreFloat4(&m_vWorldPlanes[2], XMPlaneFromPoints(vPoints[4], vPoints[5], vPoints[1]));
	XMStoreFloat4(&m_vWorldPlanes[3], XMPlaneFromPoints(vPoints[3], vPoints[2], vPoints[6]));
	XMStoreFloat4(&m_vWorldPlanes[4], XMPlaneFromPoints(vPoints[5], vPoints[4], vPoints[7]));
	XMStoreFloat4(&m_vWorldPlanes[5], XMPlaneFromPoints(vPoints[0], vPoints[1], vPoints[2]));

	Safe_Release(pPipeLine);
}

void CFrustum::Transform_To_LocalSpace(_fmatrix WorldMatrix)
{
	_matrix WorldMatrixInv = XMMatrixInverse(nullptr, WorldMatrix);

	for (_uint i = 0; i < 6; ++i)
		XMStoreFloat4(&m_vLocalPlanes[i], XMPlaneTransform(XMLoadFloat4(&m_vWorldPlanes[i]), WorldMatrixInv));
}

_bool CFrustum::isIn_WorldSpace(_fvector vWorldPosition, _float fRange)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vWorldPosition)))
			return false;
	}
	return true;
}

_bool CFrustum::isIn_LocalSpace(_fvector vLocalPosition, _float fRange)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vLocalPlanes[i]), vLocalPosition)))
			return false;
	}
	return true;
}

void CFrustum::Free()
{
}
