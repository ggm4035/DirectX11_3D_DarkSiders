#include "CMesh.h"

#include "CBone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
	, m_iMaterialsIndex(rhs.m_iMaterialsIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_vecBoneIndices(rhs.m_vecBoneIndices)
{
	strcpy_s(m_szName, rhs.m_szName);
}

void CMesh::Get_Matrices(const CModel::BONES& vecBones, OUT _float4x4* pMatrix, _fmatrix PivotMatrix)
{
	_uint iIndex = { 0 };

	for (auto iBoneIndex : m_vecBoneIndices)
	{
		XMStoreFloat4x4(&pMatrix[iIndex++], vecBones[iBoneIndex]->Get_OffsetMatrix() * vecBones[iBoneIndex]->Get_NotMoveCombinedTransformationMatrix() * PivotMatrix);
	}
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const MESHDATA& MeshData)
{
	strcpy_s(m_szName, MeshData.szName);
	m_iMaterialsIndex = MeshData.iMaterialIndex;

	m_iNumBones = MeshData.iNumMeshBones;
	m_vecBoneIndices.reserve(m_iNumBones);

	for (_uint i = 0; i < m_iNumBones; ++i)
		m_vecBoneIndices.push_back(MeshData.pBoneIndices[i]);

	m_iVertexBuffers = { 1 };
	m_iIndexStride = { sizeof(_ulong) };
	m_eFormat = { DXGI_FORMAT_R32_UINT };
	m_iNumIndices = { MeshData.iNumIndices };
	m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	HRESULT hr = { 0 };

	/* VERTEX BUFFER */
	if (CModel::TYPE_NONANIM == eModelType)
		Ready_VertexBuffer_NonAnim(MeshData);
	else
		Ready_VertexBuffer_Anim(MeshData);

	/* INDEX BUFFER */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iNumIndices * m_iIndexStride };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };
	m_BufferDesc.StructureByteStride = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = MeshData.pIndices;

	if (FAILED(Create_Buffer(&m_pIB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(const MESHDATA& MeshData)
{
	m_iNumVertices = { MeshData.iNumNonAnimVertices };
	m_iStride = { sizeof(VTXMESH) };

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	VTXMESH* pVertices = MeshData.pNonAnimVertices;
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Create_Buffer(&m_pVB)))
		return E_FAIL;

	_ulong* pIndices = MeshData.pIndices;
	m_vecTriangle.reserve(m_iNumIndices);

	for (_uint i = 0; i < m_iNumIndices;)
	{
		TRIANGLE TriangleDesc;
		ZeroMemory(&TriangleDesc, sizeof TriangleDesc);

		TriangleDesc.vDot[0] = pVertices[pIndices[i++]].vPosition;
		TriangleDesc.vDot[1] = pVertices[pIndices[i++]].vPosition;
		TriangleDesc.vDot[2] = pVertices[pIndices[i++]].vPosition;

		m_vecTriangle.push_back(TriangleDesc);
	}

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(const MESHDATA& MeshData)
{
	m_iNumVertices = { MeshData.iNumAnimVertices };
	m_iStride = { sizeof(VTXANIMMESH) };

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	VTXANIMMESH* pVertices = MeshData.pAnimVertices;
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Create_Buffer(&m_pVB)))
		return E_FAIL;

	_ulong* pIndices = MeshData.pIndices;
	m_vecTriangle.reserve(m_iNumIndices);

	for (_uint i = 0; i < m_iNumIndices;)
	{
		TRIANGLE TriangleDesc;
		ZeroMemory(&TriangleDesc, sizeof TriangleDesc);

		TriangleDesc.vDot[0] = pVertices[pIndices[i++]].vPosition;
		TriangleDesc.vDot[1] = pVertices[pIndices[i++]].vPosition;
		TriangleDesc.vDot[2] = pVertices[pIndices[i++]].vPosition;

		m_vecTriangle.push_back(TriangleDesc);
	}

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const MESHDATA& MeshData)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, MeshData)))
	{
		MSG_BOX("Failed to Created CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	CVIBuffer::Free();
}
