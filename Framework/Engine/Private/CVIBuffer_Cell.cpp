#include "CVIBuffer_Cell.h"

#include "CBounding_Sphere.h"

CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell& rhs)
	: CVIBuffer(rhs)
{
}

void CVIBuffer_Cell::Set_SpherePosition(const _uint& iIndex, const _float3 vPosition)
{
	m_pSphere[iIndex]->Set_Position(vPosition);
}

HRESULT CVIBuffer_Cell::Initialize_Prototype(const _float3* pPoints, class CBounding_Sphere* pPrototype)
{
	if (nullptr != pPrototype)
	{
		CBounding_Sphere::SPHEREDESC Desc;
		Desc.fRadius = 0.5f;

		for (_uint i = 0; i < 3; ++i)
		{
			Desc.vPosition = pPoints[i];
			m_pSphere[i] = pPrototype->Clone(&Desc);
		}
	}

	/* Buffer Init */
	m_iVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOS) };
	m_iNumVertices = 3;
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = 4;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

	/*=================*/
	//  Vertex_Buffer  //
	/*=================*/

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };

	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];

	for (_uint i = 0; i < 3; ++i)
		pVertices[i].vPosition = pPoints[i];

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(CVIBuffer::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	/*================*/
	//  Index_Buffer  //
	/*================*/

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };

	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_ushort* pIndices = new _ushort[m_iNumIndices];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(CVIBuffer::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Cell::Initialize(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CVIBuffer::Initialize(iLayerIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Cell::Render()
{
	if (FAILED(CVIBuffer::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Cell::Render_Sphere()
{
	for (_uint i = 0; i < 3; ++i)
	{
		if (nullptr == m_pSphere[i])
			return E_FAIL;

		m_pSphere[i]->Render();
	}

	return S_OK;
}

_float3 CVIBuffer_Cell::Picking_Sphere(_fvector vOrigin, _fvector vDirection, OUT _int& iSphereIndex)
{
	_float fDistance = { 0.f };
	_float fNearDistance = { FLT_MAX };
	iSphereIndex = { -1 };

	for (_uint i = 0; i < 3; ++i)
	{
		if (true == m_pSphere[i]->Get_BoundingSphere()->Intersects(vOrigin, vDirection, fDistance))
			iSphereIndex = i;
	}

	if (-1 == iSphereIndex)
		return _float3(-1.f, -1.f, -1.f);

	return m_pSphere[iSphereIndex]->Get_BoundingSphere()->Center;
}

CVIBuffer_Cell* CVIBuffer_Cell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, class CBounding_Sphere* pPrototype)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pPoints, pPrototype)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Cell* CVIBuffer_Cell::Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(*this);

	if (FAILED(pInstance->Initialize(iLayerIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cell::Free()
{
	for (_uint i = 0; i < 3; ++i)
		Safe_Release(m_pSphere[i]);

	CVIBuffer::Free();
}
