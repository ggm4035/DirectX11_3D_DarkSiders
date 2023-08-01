#include "CVIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
	m_iVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSTEX) };
	m_iNumVertices = 2 * (m_iNumRect + 1);
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = 6 * m_iNumRect;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	_uint iIndex = { 0 };
	for (_uint iRectIndex = 0; iRectIndex <= m_iNumRect; ++iRectIndex)
	{
		_float fTexU = (1.f / m_iNumRect) * _float(iRectIndex);

		pVertices[iIndex].vPosition = _float3();
		pVertices[iIndex].vTexCoord = _float2(fTexU , 0.f);

		++iIndex;

		pVertices[iIndex].vPosition = _float3();
		pVertices[iIndex].vTexCoord = _float2(fTexU, 1.f);
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

	_ushort* pIndices = new _ushort[m_iNumIndices];

	_uint iNumIndices = { 0 };
	iIndex = { 0 };
	for (_uint iRectIndex = 0; iRectIndex < m_iNumRect; ++iRectIndex)
	{
		pIndices[iNumIndices++] = iIndex;
		pIndices[iNumIndices++] = iIndex + 3;
		pIndices[iNumIndices++] = iIndex + 1;

		pIndices[iNumIndices++] = iIndex;
		pIndices[iNumIndices++] = iIndex + 2;
		pIndices[iNumIndices++] = iIndex + 3;

		iIndex += 2;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(CVIBuffer::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CVIBuffer::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	TRAILDESC Desc = *reinterpret_cast<TRAILDESC*>(pArg);

	m_iNumRect = Desc.iNumRect;
	pLowPosition = Desc.pLowPosition;
	pHightPosition = Desc.pHightPosition;
	pParentWorldMatrix = Desc.pParentWorldMatrix;

	return S_OK;
}

HRESULT CVIBuffer_Trail::Render()
{
	if (FAILED(CVIBuffer::Render()))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Trail::Tick(const _double& TimeDelta)
{
	VTXPOSTEX* pVertices = { nullptr };
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &m_MappedSubResource);

	pVertices = reinterpret_cast<VTXPOSTEX*>(m_MappedSubResource.pData);

	pVertices[0].vPosition = *pHightPosition;//XMVector3TransformCoord(XMLoadFloat3(pHightPosition), XMLoadFloat4x4(pParentWorldMatrix));
	pVertices[1].vPosition = *pLowPosition;

	_uint iIndex = { 2 };
	for (_uint i = 0; i < m_iNumRect; ++i)
	{
		pVertices[iIndex++].vPosition = pVertices[iIndex - 2].vPosition;
		pVertices[iIndex++].vPosition = pVertices[iIndex - 2].vPosition;
	}

	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Trail* CVIBuffer_Trail::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	CVIBuffer::Free();
}
