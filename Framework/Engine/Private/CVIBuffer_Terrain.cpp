#include "CVIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	, m_iXCount(rhs.m_iXCount)
	, m_iZCount(rhs.m_iZCount)
	, m_fInterval(rhs.m_fInterval)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iXCount, _uint iZCount, _float fInterval)
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	m_iXCount = iXCount;
	m_iZCount = iZCount;
	m_fInterval = fInterval;

	m_iVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSNORTEX) };
	m_iNumVertices = m_iXCount * m_iZCount;
	m_iIndexStride = { sizeof(_uint) };
	m_iNumIndices = (m_iXCount - 1) * (m_iZCount - 1) * 6;
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*=================*/
	//  Vertex_Buffer  //
	/*=================*/

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOSNORTEX* pVertices = new VTXPOSNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iZCount; ++i)
	{
		for (_uint j = 0; j < m_iXCount; ++j)
		{
			_uint iIndex = i * m_iXCount + j;

			pVertices[iIndex].vPosition = _float3(
				(_float)j * m_fInterval,
				0.f,
				(_float)i * m_fInterval);
			pVertices[iIndex].vTexCoord = _float2(
				(_float)j / (_float)(m_iXCount - 1),
				(_float)i / (_float)(m_iZCount - 1));
		}
	}
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

	_uint* pIndices = new _uint[(m_iNumIndices + 1)];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint iCountIndex = 0;

	for (_uint i = 0; i < m_iZCount - 1; ++i)
	{
		for (_uint j = 0; j < m_iXCount - 1; ++j)
		{
			_uint iIndex = i * m_iXCount + j;
	
			pIndices[iCountIndex] = iIndex; iCountIndex++;
			pIndices[iCountIndex] = iIndex + m_iXCount + 1; iCountIndex++;
			pIndices[iCountIndex] = iIndex + 1; iCountIndex++;

			pIndices[iCountIndex] = iIndex; iCountIndex++;
			pIndices[iCountIndex] = iIndex + m_iXCount; iCountIndex++;
			pIndices[iCountIndex] = iIndex + m_iXCount + 1; iCountIndex++;
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(CVIBuffer::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Render()
{
	if (FAILED(CVIBuffer::Render()))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	_uint iXCount, _uint iZCount, _float fInterval)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iXCount, iZCount, fInterval)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	CVIBuffer::Free();
}
