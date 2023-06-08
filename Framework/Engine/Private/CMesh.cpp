#include "CMesh.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	:CVIBuffer(rhs)
{
}

HRESULT CMesh::Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	m_strName = pAIMesh->mName.data;
	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	m_iVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXMESH) };
	m_iNumVertices = { pAIMesh->mNumVertices };
	if (m_iNumVertices < 65500)
	{
		m_iIndexStride = { sizeof(_ushort) };
		m_eFormat = { DXGI_FORMAT_R16_UINT };
	}
	else
	{
		m_iIndexStride = { sizeof(_ulong) };
		m_eFormat = { DXGI_FORMAT_R32_UINT };
	}
	m_iNumIndices = { pAIMesh->mNumFaces * 3 };
	m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	/* VERTEX BUFFER */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };
	m_BufferDesc.StructureByteStride = { m_iStride };

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, 
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

										/* 뭔가 밀리는거 같으면 텍스처 인덱스를 조절해보자. */
		memcpy(&pVertices[i].vTexCoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	/* INDEX BUFFER */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iNumIndices * m_iIndexStride };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };
	m_BufferDesc.StructureByteStride = { 0 };

	_uint iNumIndices = { 0 };
	if (m_iNumVertices < 65500)
	{
		_ushort* pIndices = new _ushort[m_iNumIndices];

		for (_uint i = 0; i < pAIMesh->mNumFaces; ++i)
		{
			pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
			pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
			pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
		}

		ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
		m_SubResourceData.pSysMem = pIndices;

		if (FAILED(Create_Buffer(&m_pIB)))
			return E_FAIL;

		Safe_Delete_Array(pIndices);
	}
	else
	{
		_ulong* pIndices = new _ulong[m_iNumIndices];

		for (_uint i = 0; i < pAIMesh->mNumFaces; ++i)
		{
			pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
			pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
			pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
		}

		ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
		m_SubResourceData.pSysMem = pIndices;

		if (FAILED(Create_Buffer(&m_pIB)))
			return E_FAIL;

		Safe_Delete_Array(pIndices);
	}

	return S_OK;
}

HRESULT CMesh::Initialize(CComponent* pOwner, void* pArg)
{
	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pAIMesh, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Clone(CComponent * pOwner, void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
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
