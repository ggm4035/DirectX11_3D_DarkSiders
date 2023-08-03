#include "CInstance_Rect.h"

CInstance_Rect::CInstance_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer_Instancing(pDevice, pContext)
{
}

CInstance_Rect::CInstance_Rect(const CInstance_Rect& rhs)
	:CVIBuffer_Instancing(rhs)
{
}

HRESULT CInstance_Rect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInstance_Rect::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CVIBuffer_Instancing::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	//m_iIndexCountPerInstance = 6;
	//m_iVertexBuffers = { 2 };
	//m_iStride = { sizeof(VTXPOSTEX) };
	//m_iNumVertices = 4;
	//m_iIndexStride = { sizeof(_ushort) };
	//m_iNumIndices = { m_iIndexCountPerInstance * m_iNumInstance };
	//m_eFormat = DXGI_FORMAT_R16_UINT;
	//m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	///*=================*/
	////  Vertex_Buffer  //
	///*=================*/

	//ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	//m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	//m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	//m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	//m_BufferDesc.StructureByteStride = { m_iStride };

	//m_BufferDesc.CPUAccessFlags = { 0 };
	//m_BufferDesc.MiscFlags = { 0 };

	//VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];

	//pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	//pVertices[0].vTexCoord = _float2(0.f, 0.f);

	//pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	//pVertices[1].vTexCoord = _float2(1.f, 0.f);

	//pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	//pVertices[2].vTexCoord = _float2(1.f, 1.f);

	//pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	//pVertices[3].vTexCoord = _float2(0.f, 1.f);

	//ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	//m_SubResourceData.pSysMem = pVertices;

	//if (FAILED(CVIBuffer::Create_Buffer(&m_pVB)))
	//	return E_FAIL;

	//Safe_Delete_Array(pVertices);

	///*================*/
	////  Index_Buffer  //
	///*================*/

	//ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	//m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	//m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	//m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	//m_BufferDesc.StructureByteStride = { 0 };

	//m_BufferDesc.CPUAccessFlags = { 0 };
	//m_BufferDesc.MiscFlags = { 0 };

	//_ushort* pIndices = new _ushort[m_iNumIndices];
	//ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	//_uint iNumIndices = { 0 };
	//for (_uint i = 0; i < m_iNumInstance; ++i)
	//{
	//	pIndices[iNumIndices++] = 0;
	//	pIndices[iNumIndices++] = 1;
	//	pIndices[iNumIndices++] = 2;

	//	pIndices[iNumIndices++] = 0;
	//	pIndices[iNumIndices++] = 2;
	//	pIndices[iNumIndices++] = 3;
	//}

	//ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	//m_SubResourceData.pSysMem = pIndices;

	//if (FAILED(CVIBuffer::Create_Buffer(&m_pIB)))
	//	return E_FAIL;

	//Safe_Delete_Array(pIndices);

	return S_OK;
}

void CInstance_Rect::Free()
{
	CVIBuffer_Instancing::Free();
}
