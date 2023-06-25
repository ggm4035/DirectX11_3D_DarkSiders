#include "CCell.h"

#if defined(_USE_IMGUI) || defined(_DEBUG)
#include "CVIBuffer_Cell.h"
#include "CGameInstance.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	ZeroMemory(m_arrPoints, sizeof(_float3) * 3);
}

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

CCell::RETURNDESC CCell::is_In(_fvector vPosition)
{
	RETURNDESC RetDesc;

	for (_uint i = 0; i < NEIGHBOR_END; ++i)
	{
		_vector vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_arrPoints[i]));
		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&m_arrNormals[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vDir, vNormal))) /* 현재 셀을 나간 경우 */
		{
			RetDesc.vecNeighborIndex.push_back(m_arrNeighborIndex[i]);

			if (-1 == m_arrNeighborIndex[i])
			{
				switch (i)
				{
				case NEIGHBOR_AB:
					XMStoreFloat3(&RetDesc.vSlide, XMVector3Normalize(XMLoadFloat3(&m_arrPoints[POINT_B]) - XMLoadFloat3(&m_arrPoints[POINT_A])));
					break;

				case NEIGHBOR_BC:
					XMStoreFloat3(&RetDesc.vSlide, XMVector3Normalize(XMLoadFloat3(&m_arrPoints[POINT_C]) - XMLoadFloat3(&m_arrPoints[POINT_B])));
					break;

				case NEIGHBOR_CA:
					XMStoreFloat3(&RetDesc.vSlide, XMVector3Normalize(XMLoadFloat3(&m_arrPoints[POINT_A]) - XMLoadFloat3(&m_arrPoints[POINT_C])));
					break;
				}
			}
		}
	}

	return RetDesc;
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
#if defined(_USE_IMGUI) || defined(_DEBUG)
	Safe_Release(m_pBuffer);
#endif
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

#if defined(_USE_IMGUI) || defined(_DEBUG) 

void CCell::Set_Position(POINT ePoint, const _float3& vPosition)
{
	m_arrPoints[ePoint] = vPosition;

	D3D11_MAPPED_SUBRESOURCE SubResource;

	m_pBuffer->Begin(SubResource);

	VTXPOS* pVertices = reinterpret_cast<VTXPOS*>(SubResource.pData);

	pVertices[ePoint].vPosition = vPosition;

	m_pBuffer->End();

	m_pBuffer->Set_SpherePosition((_uint)ePoint, vPosition);
}

HRESULT CCell::Initialize(const _float3* pPoints, const _int& iIndex, CBounding_Sphere* pPrototype)
{
	Initialize(pPoints, iIndex);

	m_pBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_arrPoints, pPrototype);
	if (nullptr == m_pBuffer)
		return E_FAIL;

	return S_OK;
}

HRESULT CCell::Render()
{
	if (nullptr == m_pBuffer)
		return E_FAIL;

	m_pBuffer->Render();

	return S_OK;
}

_bool CCell::Picking_Spheres(_fvector vOrigin, _fvector vDirection, OUT _float3& _vPosition, OUT _int& iSphereIndex)
{
	_float3 vPosition = m_pBuffer->Picking_Sphere(vOrigin, vDirection, iSphereIndex);

	if (-1 == iSphereIndex)
	{
		_vPosition = _float3(-1.f, -1.f, -1.f);
		return false;
	}

	_vPosition = vPosition;

	return true;
}

HRESULT CCell::Render_Sphere()
{
	if (FAILED(m_pBuffer->Render_Sphere()))
		return E_FAIL;

	return S_OK;
}

void CCell::Sort()
{
	_vector vLineAB = XMLoadFloat3(&m_arrPoints[POINT_B]) - XMLoadFloat3(&m_arrPoints[POINT_A]);
	_vector vLineAC = XMLoadFloat3(&m_arrPoints[POINT_C]) - XMLoadFloat3(&m_arrPoints[POINT_A]);

	_vector vNormal = XMVector3Cross(vLineAB, vLineAC);

	if (0 > vNormal.m128_f32[1])
	{
		_float3 vTemp = m_arrPoints[POINT_B];
		m_arrPoints[POINT_B] = m_arrPoints[POINT_C];
		m_arrPoints[POINT_C] = vTemp;

		vTemp = m_arrNormals[NEIGHBOR_AB];
		m_arrNormals[NEIGHBOR_AB] = m_arrNormals[NEIGHBOR_CA];
		m_arrNormals[NEIGHBOR_CA] = vTemp;
	}
}

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, const _int& iIndex, class CBounding_Sphere* pPrototype)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex, pPrototype)))
	{
		MSG_BOX("Failed to Create CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}


#endif // _USE_IMGUI