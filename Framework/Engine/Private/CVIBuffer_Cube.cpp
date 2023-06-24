#include "CVIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	m_iVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSCOL) };
	m_iNumVertices = { 8 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { 36 };
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	VTXPOSCOL* pVertices = new VTXPOSCOL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSCOL) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vColor = _float4(0.f, 1.f, 1.f, 1.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vColor = _float4(0.f, 1.f, 1.f, 1.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vColor = _float4(0.f, 1.f, 1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vColor = _float4(0.f, 1.f, 1.f, 1.f);

	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vColor = _float4(0.f, 1.f, 1.f, 1.f);

	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vColor = _float4(0.f, 1.f, 1.f, 1.f);

	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vColor = _float4(0.f, 1.f, 1.f, 1.f);

	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vColor = _float4(0.f, 1.f, 1.f, 1.f);

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);
	TRIANGLE TriangleDesc;
	ZeroMemory(&TriangleDesc, sizeof TriangleDesc);

	pIndices[0] = 1;
	pIndices[1] = 5;
	pIndices[2] = 6;

	TriangleDesc.vDot[0] = pVertices[1].vPosition;
	TriangleDesc.vDot[1] = pVertices[5].vPosition;
	TriangleDesc.vDot[2] = pVertices[6].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	pIndices[3] = 1;
	pIndices[4] = 6;
	pIndices[5] = 2;

	TriangleDesc.vDot[0] = pVertices[1].vPosition;
	TriangleDesc.vDot[1] = pVertices[6].vPosition;
	TriangleDesc.vDot[2] = pVertices[2].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	pIndices[6] = 4;
	pIndices[7] = 0;
	pIndices[8] = 3;

	TriangleDesc.vDot[0] = pVertices[4].vPosition;
	TriangleDesc.vDot[1] = pVertices[0].vPosition;
	TriangleDesc.vDot[2] = pVertices[3].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	pIndices[9] = 4;
	pIndices[10] = 3;
	pIndices[11] = 7;

	TriangleDesc.vDot[0] = pVertices[4].vPosition;
	TriangleDesc.vDot[1] = pVertices[3].vPosition;
	TriangleDesc.vDot[2] = pVertices[7].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	pIndices[12] = 4;
	pIndices[13] = 5;
	pIndices[14] = 1;

	TriangleDesc.vDot[0] = pVertices[4].vPosition;
	TriangleDesc.vDot[1] = pVertices[5].vPosition;
	TriangleDesc.vDot[2] = pVertices[1].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	pIndices[15] = 4;
	pIndices[16] = 1;
	pIndices[17] = 0;

	TriangleDesc.vDot[0] = pVertices[4].vPosition;
	TriangleDesc.vDot[1] = pVertices[1].vPosition;
	TriangleDesc.vDot[2] = pVertices[0].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	pIndices[18] = 3;
	pIndices[19] = 2;
	pIndices[20] = 6;

	TriangleDesc.vDot[0] = pVertices[3].vPosition;
	TriangleDesc.vDot[1] = pVertices[2].vPosition;
	TriangleDesc.vDot[2] = pVertices[6].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	pIndices[21] = 3;
	pIndices[22] = 6;
	pIndices[23] = 7;

	TriangleDesc.vDot[0] = pVertices[3].vPosition;
	TriangleDesc.vDot[1] = pVertices[6].vPosition;
	TriangleDesc.vDot[2] = pVertices[7].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	pIndices[24] = 7;
	pIndices[25] = 6;
	pIndices[26] = 5;

	TriangleDesc.vDot[0] = pVertices[7].vPosition;
	TriangleDesc.vDot[1] = pVertices[6].vPosition;
	TriangleDesc.vDot[2] = pVertices[5].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	pIndices[27] = 7;
	pIndices[28] = 5;
	pIndices[29] = 4;

	TriangleDesc.vDot[0] = pVertices[7].vPosition;
	TriangleDesc.vDot[1] = pVertices[5].vPosition;
	TriangleDesc.vDot[2] = pVertices[4].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	pIndices[30] = 0;
	pIndices[31] = 1;
	pIndices[32] = 2;

	TriangleDesc.vDot[0] = pVertices[0].vPosition;
	TriangleDesc.vDot[1] = pVertices[1].vPosition;
	TriangleDesc.vDot[2] = pVertices[2].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	pIndices[33] = 0;
	pIndices[34] = 2;
	pIndices[35] = 3;

	TriangleDesc.vDot[0] = pVertices[0].vPosition;
	TriangleDesc.vDot[1] = pVertices[2].vPosition;
	TriangleDesc.vDot[2] = pVertices[3].vPosition;
	m_vecTriangle.push_back(TriangleDesc);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Cube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Cube::Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(iLayerIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Cube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}