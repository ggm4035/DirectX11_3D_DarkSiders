#include "CMesh.h"

#include "CBone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer(pDevice, pContext)
{
	ZeroMemory(&m_MeshDataDesc, sizeof m_MeshDataDesc);
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
	, m_MeshDataDesc(rhs.m_MeshDataDesc)
	, m_iNumBones(rhs.m_iNumBones)
	, m_BoneIndices(rhs.m_BoneIndices)
{
}

void CMesh::Get_Matrices(const CModel::BONES& vecBones, OUT _float4x4* pMatrix)
{
	_uint iIndex = { 0 };

	for (auto iBoneIndex : m_BoneIndices)
	{
		XMStoreFloat4x4(&pMatrix[iIndex++], vecBones[iBoneIndex]->Get_OffsetMatrix() * vecBones[iBoneIndex]->Get_CombinedTransformationMatrix());
	}
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const CModel::BONES& vecBones, const MESHDATA& MeshData, _fmatrix PivotMatrix)
{
	m_MeshDataDesc = MeshData;

	m_iVertexBuffers = { 1 };

	m_iNumVertices = { m_MeshDataDesc.iNumVertices };
	m_iIndexStride = { sizeof(_ulong) };
	m_eFormat = { DXGI_FORMAT_R32_UINT };
	m_iNumIndices = { m_MeshDataDesc.iNumIndices };
	m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	HRESULT hr = { 0 };

	if (CModel::TYPE_NONANIM == eModelType)
		Ready_VertexBuffer_NonAnim(m_MeshDataDesc, PivotMatrix);
	else
		;//Ready_VertexBuffer_Anim(MeshData, vecBones);

	/* INDEX BUFFER */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iNumIndices * m_iIndexStride };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };
	m_BufferDesc.StructureByteStride = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_MeshDataDesc.pIndices;

	if (FAILED(Create_Buffer(&m_pIB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Initialize(CComponent* pOwner, void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(const MESHDATA& MeshData, _fmatrix PivotMatrix)
{
	m_iStride = { sizeof(VTXMESH) };

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };
	m_BufferDesc.StructureByteStride = { m_iStride };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = MeshData.pVertices;

	if (FAILED(Create_Buffer(&m_pVB)))
		return E_FAIL;

	return S_OK;
}

// 오늘 해야되는 거임
//HRESULT CMesh::Ready_VertexBuffer_Anim(const MESHDATA& MeshData, const CModel::BONES& vecBones)
//{
//	m_iStride = { sizeof(VTXANIMMESH) };
//
//	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
//	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
//	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
//	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
//	m_BufferDesc.StructureByteStride = { m_iStride };
//	m_BufferDesc.CPUAccessFlags = { 0 };
//	m_BufferDesc.MiscFlags = { 0 };
//
//	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
//	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);
//
//	for (_uint i = 0; i < m_iNumVertices; ++i)
//	{
//		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
//		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
//		memcpy(&pVertices[i].vTexCoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
//		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
//	}
//
//	m_iNumBones = pAIMesh->mNumBones;
//
//	for (_uint i = 0; i < m_iNumBones; ++i)
//	{
//		aiBone* pAIBone = pAIMesh->mBones[i];
//
//		_uint iBoneIndex = { 0 };
//
//		auto& iter = find_if(vecBones.begin(), vecBones.end(), [&](const CBone* pValue)
//			{
//				if (pValue->Get_Name() == pAIBone->mName.data)
//					return true;
//				else
//				{
//					++iBoneIndex;
//					return false;
//				}
//			});
//
//		_float4x4 OffsetMatrix;
//		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
//		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
//
//		vecBones[iBoneIndex]->Set_OffsetMatrix(OffsetMatrix);
//
//		m_BoneIndices.push_back(iBoneIndex);
//
//		/* pAIBone->mNumWeights : 이뼈는 몇개 정점에게 영향을 주니. */
//		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
//		{
//			/* AIVertexWeight : j번째 정점에게 영향을 주는정보. */
//			aiVertexWeight	AIVertexWeight = pAIBone->mWeights[j];
//
//			if (0.0f == pVertices[AIVertexWeight.mVertexId].vBlendWeights.x)
//			{
//				pVertices[AIVertexWeight.mVertexId].vBlendIndices.x = i;
//				pVertices[AIVertexWeight.mVertexId].vBlendWeights.x = AIVertexWeight.mWeight;
//			}
//
//			else if (0.0f == pVertices[AIVertexWeight.mVertexId].vBlendWeights.y)
//			{
//				pVertices[AIVertexWeight.mVertexId].vBlendIndices.y = i;
//				pVertices[AIVertexWeight.mVertexId].vBlendWeights.y = AIVertexWeight.mWeight;
//			}
//
//			else if (0.0f == pVertices[AIVertexWeight.mVertexId].vBlendWeights.z)
//			{
//				pVertices[AIVertexWeight.mVertexId].vBlendIndices.z = i;
//				pVertices[AIVertexWeight.mVertexId].vBlendWeights.z = AIVertexWeight.mWeight;
//			}
//
//			else if (0.0f == pVertices[AIVertexWeight.mVertexId].vBlendWeights.w)
//			{
//				pVertices[AIVertexWeight.mVertexId].vBlendIndices.w = i;
//				pVertices[AIVertexWeight.mVertexId].vBlendWeights.w = AIVertexWeight.mWeight;
//			}
//		}
//	}
//
//	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
//	m_SubResourceData.pSysMem = pVertices;
//
//	if (FAILED(Create_Buffer(&m_pVB)))
//		return E_FAIL;
//
//	Safe_Delete_Array(pVertices);
//
//	return S_OK;
//}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const CModel::BONES& vecBones, const MESHDATA& MeshData, _fmatrix PivotMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, vecBones, MeshData, PivotMatrix)))
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
