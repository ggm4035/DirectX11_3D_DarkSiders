#include "CVIBuffer_Coordnate.h"

CVIBuffer_Coordnate::CVIBuffer_Coordnate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Coordnate::CVIBuffer_Coordnate(const CVIBuffer_Coordnate& rhs)
	:CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Coordnate::Initialize_Prototype()
{
	m_iVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSCOL) };
	m_iNumVertices = 4; // 사각형이니까 버텍스 개수는 4개읾
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = 6;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	/*=================*/
	//  Vertex_Buffer  //
	/*=================*/

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iNumVertices * m_iStride };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };

	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOSCOL* pVertices = new VTXPOSCOL[m_iNumVertices];

	pVertices[0].vPosition = _float3(0.f, 0.001f, 0.f);
	pVertices[0].vColor = _float4(0.f, 0.f, 0.f, 1.f);

	pVertices[1].vPosition = _float3(1.f, 0.f, 0.f);
	pVertices[1].vColor = _float4(1.f, 0.f, 0.f, 1.f);

	pVertices[2].vPosition = _float3(0.f, 1.f, 0.f);
	pVertices[2].vColor = _float4(0.f, 1.f, 0.f, 1.f);

	pVertices[3].vPosition = _float3(0.f, 0.f, 1.f);
	pVertices[3].vColor = _float4(0.f, 0.f, 1.f, 1.f);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(CVIBuffer::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	/*================*/
	//  Index_Buffer  //
	/*================*/

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iNumIndices * m_iIndexStride };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = 0;

	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_ushort* pIndices = new _ushort[m_iNumIndices];

	pIndices[0] = 0;
	pIndices[1] = 1;

	pIndices[2] = 0;
	pIndices[3] = 2;

	pIndices[4] = 0;
	pIndices[5] = 3;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(CVIBuffer::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Coordnate::Initialize(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CVIBuffer::Initialize(iLayerIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Coordnate::Render()
{
	if (FAILED(CVIBuffer::Render()))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Coordnate* CVIBuffer_Coordnate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Coordnate* pInstance = new CVIBuffer_Coordnate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Coordnate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer_Coordnate* CVIBuffer_Coordnate::Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	CVIBuffer_Coordnate* pInstance = new CVIBuffer_Coordnate(*this);

	if (FAILED(pInstance->Initialize(iLayerIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Coordnate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Coordnate::Free()
{
	CVIBuffer::Free();
}
