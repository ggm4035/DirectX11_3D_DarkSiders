#include "CVIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Instancing::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CVIBuffer::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		m_iNumInstance = reinterpret_cast<INSTANCEDESC*>(pArg)->iNumInstance;

	/* 인스턴스 정점 버퍼를 생성한다.(m_pVBInstance) */
	D3D11_BUFFER_DESC		BufferDesc;
	ZeroMemory(&BufferDesc, sizeof BufferDesc);

	m_iInstanceStride = sizeof(VTXINSTANCE);

	BufferDesc.ByteWidth = { m_iInstanceStride * m_iNumInstance };
	BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	BufferDesc.StructureByteStride = { m_iInstanceStride };
	BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	BufferDesc.MiscFlags = { 0 };

	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pVertices[i].vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	}

	D3D11_SUBRESOURCE_DATA		SubResourceData;
	ZeroMemory(&SubResourceData, sizeof SubResourceData);

	SubResourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

void CVIBuffer_Instancing::Tick(const vector<_float4x4>& vecMatrices)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXINSTANCE* pVertices = static_cast<VTXINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		memcpy(&pVertices[i].vRight, vecMatrices[i].m[0], sizeof(_float4));
		memcpy(&pVertices[i].vUp, vecMatrices[i].m[1], sizeof(_float4));
		memcpy(&pVertices[i].vLook, vecMatrices[i].m[2], sizeof(_float4));
		memcpy(&pVertices[i].vTranslation, vecMatrices[i].m[3], sizeof(_float4));
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Instancing::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iInstanceStride,
	};

	_uint					iOffset[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iVertexBuffers, pBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instancing::Free()
{
	CVIBuffer::Free();

	Safe_Release(m_pVBInstance);
}
