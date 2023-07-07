#include "CVIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	:CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype(const INSTANCEDESC* pInstanceDesc)
{
	m_iNumInstance = pInstanceDesc->iNumInstance;
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (nullptr != pArg)
	{
		m_iNumInstance = reinterpret_cast<INSTANCEDESC*>(pArg)->iNumInstance;
	}

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof BufferDesc);

	m_iInstanceStride = sizeof(VTXINSTANCE);

	BufferDesc.ByteWidth = { m_iInstanceStride * m_iNumInstance };
	BufferDesc.StructureByteStride = { m_iInstanceStride };
	BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	BufferDesc.MiscFlags = { 0 };
	
	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pVertices[i].vRight =		_float4(1.f, 0.f, 0.f, 0.f);
		pVertices[i].vUp =			_float4(0.f, 1.f, 0.f, 0.f);
		pVertices[i].vLook =		_float4(0.f, 0.f, 1.f, 0.f);
		pVertices[i].vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	}

	D3D11_SUBRESOURCE_DATA SubResourceData;
	ZeroMemory(&SubResourceData, sizeof SubResourceData);

	SubResourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pBuffer[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint iStrides[] = {
		m_iStride,
		m_iInstanceStride,
	};

	_uint iOffset[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertices, pBuffer, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Begin_Instance(OUT VTXINSTANCE** ppSubResourceData)
{
	if (nullptr == m_pVBInstance || 
		nullptr == m_pContext)
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	*ppSubResourceData = reinterpret_cast<VTXINSTANCE*>(SubResource.pData);

	return S_OK;
}

void CVIBuffer_Instancing::End_Instance()
{
	if (nullptr != m_pContext)
		m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instancing::Free()
{
	Safe_Release(m_pVBInstance);

	CVIBuffer::Free();
}
