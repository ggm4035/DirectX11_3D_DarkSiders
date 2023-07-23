#include "CInstance_Mesh.h"

CInstance_Mesh::CInstance_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CInstance_Mesh::CInstance_Mesh(const CInstance_Mesh& rhs)
	: CVIBuffer_Instancing(rhs)
{
}

HRESULT CInstance_Mesh::Initialize_Prototype(const MESHINSTANCEDESC* pInstanceDesc)
{
	if (FAILED(CVIBuffer_Instancing::Initialize_Prototype(pInstanceDesc)))
		return E_FAIL;

	//MESHDATA MeshData = pInstanceDesc->MeshData;

	//strcpy_s(m_szName, MeshData.szName);
	//m_iMaterialsIndex = MeshData.iMaterialIndex;

	//m_iNumBones = MeshData.iNumMeshBones;
	//m_vecBoneIndices.reserve(m_iNumBones);

	//for (_uint i = 0; i < m_iNumBones; ++i)
	//	m_vecBoneIndices.push_back(MeshData.pBoneIndices[i]);

	//m_iVertexBuffers = { 1 };
	//m_iIndexStride = { sizeof(_ulong) };
	//m_eFormat = { DXGI_FORMAT_R32_UINT };
	//m_iNumIndices = { MeshData.iNumIndices };
	//m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	//HRESULT hr = { 0 };

	///* VERTEX BUFFER */
	//if (CModel::TYPE_NONANIM == eModelType)
	//	Ready_VertexBuffer_NonAnim(MeshData);
	//else
	//	Ready_VertexBuffer_Anim(MeshData);

	///* INDEX BUFFER */
	//ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	//m_BufferDesc.ByteWidth = { m_iNumIndices * m_iIndexStride };
	//m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	//m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	//m_BufferDesc.CPUAccessFlags = { 0 };
	//m_BufferDesc.MiscFlags = { 0 };
	//m_BufferDesc.StructureByteStride = { 0 };

	//ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	//m_SubResourceData.pSysMem = MeshData.pIndices;

	//if (FAILED(Create_Buffer(&m_pIB)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CInstance_Mesh::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CVIBuffer_Instancing::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

CInstance_Mesh* CInstance_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MESHINSTANCEDESC* pInstanceDesc)
{
	CInstance_Mesh* pInstance = new CInstance_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pInstanceDesc)))
	{
		MSG_BOX("Failed to Created CInstance_Mesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CInstance_Mesh* CInstance_Mesh::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CInstance_Mesh* pInstance = new CInstance_Mesh(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CInstance_Mesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInstance_Mesh::Free()
{
	CVIBuffer_Instancing::Free();
}
