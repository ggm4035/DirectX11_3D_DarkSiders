#include "CCell.h"

#ifdef _DEBUG
#include "CVIBuffer_Cell.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	ZeroMemory(m_arrPoints, sizeof(_float3) * 3);
}

#ifdef _DEBUG

HRESULT CCell::Render()
{
	if (nullptr == m_pBuffer)
		return E_FAIL;
	
	m_pBuffer->Render();

	return S_OK;
}

#endif // DEBUG

HRESULT CCell::Initialize(const _float3* pPoints, const _int& iIndex)
{
	memcpy(m_arrPoints, pPoints, sizeof(_float3) * POINT_END);

	m_iIndex = iIndex;

	_vector vLine;

	vLine = XMLoadFloat3(&m_arrPoints[POINT_B]) - XMLoadFloat3(&m_arrPoints[POINT_A]);
	m_arrNormals[NEIGHBOR_AB] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMLoadFloat3(&m_arrPoints[POINT_C]) - XMLoadFloat3(&m_arrPoints[POINT_B]);
	m_arrNormals[NEIGHBOR_BC] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMLoadFloat3(&m_arrPoints[POINT_A]) - XMLoadFloat3(&m_arrPoints[POINT_C]);
	m_arrNormals[NEIGHBOR_CA] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

#ifdef _DEBUG
	m_pBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_arrPoints);
	if (nullptr == m_pBuffer)
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_A]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_B]), vDestPoint))
			return true;
		else if (true == XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_C]), vDestPoint))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_B]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_A]), vDestPoint))
			return true;
		else if (true == XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_C]), vDestPoint))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_C]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_B]), vDestPoint))
			return true;
		else if (true == XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_A]), vDestPoint))
			return true;
	}

	return false;
}

_bool CCell::is_In(_fvector vPosition)
{
	return true;
}

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, const _int& iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Create CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pBuffer);
#endif
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}