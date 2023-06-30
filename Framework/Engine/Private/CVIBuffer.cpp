#include "CVIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB), m_pIB(rhs.m_pIB)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iNumIndices(rhs.m_iNumIndices)
	, m_iStride(rhs.m_iStride)
	, m_iIndexStride(rhs.m_iIndexStride)
	, m_BufferDesc(rhs.m_BufferDesc)
	, m_SubResourceData(rhs.m_SubResourceData)
	, m_iVertexBuffers(rhs.m_iVertexBuffers)
	, m_eFormat(rhs.m_eFormat)
	, m_eTopology(rhs.m_eTopology)
	, m_vecTriangle(rhs.m_vecTriangle)
	, m_MappedSubResource(rhs.m_MappedSubResource)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

void CVIBuffer::Set_Triangle(const _uint& iIndex, const TRIANGLE& Triangle)
{
	if (0 > iIndex || m_vecTriangle.size() <= iIndex)
		return;

	memcpy(&m_vecTriangle[iIndex], &Triangle, sizeof(TRIANGLE));
}

void CVIBuffer::Set_Triangle_Dot(const _uint& iIndex, const _uint& iDot, const _float3& vValue)
{
	if (0 > iIndex || m_vecTriangle.size() <= iIndex)
		return;

	m_vecTriangle[iIndex].vDot[iDot] = vValue;
}

HRESULT CVIBuffer::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	ID3D11Buffer* pBuffers[] = {
		m_pVB,
	};
	_uint iStrides[] = {
		m_iStride,
	};
	_uint iOffsets[] = {
		0,
	};
	m_pContext->IASetVertexBuffers(0, m_iVertexBuffers, pBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Begin(OUT D3D11_MAPPED_SUBRESOURCE& Out)
{
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &m_MappedSubResource);

	Out = m_MappedSubResource;

	return S_OK;
}

void CVIBuffer::End()
{
	m_pContext->Unmap(m_pVB, 0);
}

HRESULT CVIBuffer::Create_Buffer(OUT ID3D11Buffer** ppOut)
{
	if (nullptr == m_pDevice)
		return E_FAIL;
	
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, ppOut);
}

void CVIBuffer::Free()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	CComponent::Free();
}
