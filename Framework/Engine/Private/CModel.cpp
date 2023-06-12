#include "CModel.h"
#include "CMesh.h"
#include "CBone.h"
#include "CAnimation.h"

#include "CGameInstance.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_vecMeshes(rhs.m_vecMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_vecMaterials(rhs.m_vecMaterials)
	, m_vecBones(rhs.m_vecBones)
{
	/* 지금 얕은복사 한거고 나중에 없앨 수도 있음(릴리즈도 마찬가지) */
	for (auto& pBone : m_vecBones)
		Safe_AddRef(pBone);

	for (auto& pMesh : m_vecMeshes)
		Safe_AddRef(pMesh);

	for (auto& Meterial : m_vecMaterials)
	{
		for (auto& Texture : Meterial.pMtrlTexture)
			Safe_AddRef(Texture);
	}
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const MODEL_BINARYDATA& ModelData)
{
	if (FAILED(Ready_Bones(ModelData, nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(ModelData, eModelType)))
		return E_FAIL;

	if (FAILED(Ready_Materials(ModelData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(CComponent* pOwner, void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(const _uint& iMeshIndex)
{
	if (iMeshIndex < 0 || iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_vecMeshes[iMeshIndex]->Render();
}

void CModel::Play_Animation()
{
	/* 여기서 플레이 할 애니메이션의 인덱스 전달해주면 해당 애니메이션이 가지고 있는
	뼈(Channel)들의 TransformationMatrix를 변환시킨다. 이 후 CombinedTransformation 들을 변환 */

	

	for (auto& pBone : m_vecBones)
		pBone->Invalidate_CombinedTransformationMatrix();
}

HRESULT CModel::Bind_Material(CShader* pShader, const string& strTypename, const _uint& iMeshIndex, TEXTURETYPE eTextureType)
{
	if (nullptr == pShader ||
		iMeshIndex < 0 || iMeshIndex >= m_iNumMeshes ||
		eTextureType < 0 || eTextureType >= TEXTURE_TYPE_MAX)
		return E_FAIL;

	_uint iMaterialIndex = m_vecMeshes[iMeshIndex]->Get_MaterialIndex();

	/*if (nullptr == m_vecMaterials[iMaterialIndex].pMtrlTexture[eTextureType])
		return S_OK;*/

	return m_vecMaterials[iMaterialIndex].pMtrlTexture[eTextureType]->Bind_ShaderResource(pShader, strTypename);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const string& strTypename, const _uint& iMeshIndex)
{
	if (nullptr == pShader ||
		iMeshIndex < 0 || iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_float4x4 BoneMatrices[256];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 256);

	m_vecMeshes[iMeshIndex]->Get_Matrices(m_vecBones, BoneMatrices);

	return pShader->Bind_Float4x4_Array(strTypename, BoneMatrices, 256);
}

HRESULT CModel::Ready_Meshes(const MODEL_BINARYDATA& ModelData, TYPE eModelType)
{
	m_iNumMeshes = ModelData.iNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eModelType, ModelData.pMeshDatas[i]);
		if (nullptr == pMesh)
			return E_FAIL;

		m_vecMeshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const MODEL_BINARYDATA& ModelData)
{
	m_iNumMaterials = ModelData.iNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MESHMATERIAL MeshMaterialDesc;
		ZeroMemory(&MeshMaterialDesc, sizeof MeshMaterialDesc);
		
		for (_uint j = 0; j < TEXTURE_TYPE_MAX; ++j)
		{
			if (0 == lstrlen(ModelData.pMaterialPaths[i].szMaterialTexturePath[j]))
				continue;

			MeshMaterialDesc.pMtrlTexture[j] = CTexture::Create(m_pDevice, m_pContext, ModelData.pMaterialPaths[i].szMaterialTexturePath[j]);
			if (nullptr == MeshMaterialDesc.pMtrlTexture[j])
				return E_FAIL;
		}

		m_vecMaterials.push_back(MeshMaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const MODEL_BINARYDATA& ModelData, class CBone* pParent)
{
	if (nullptr == ModelData.pBoneDatas)
		return S_OK;

	CBone* pBone = CBone::Create(ModelData.pBoneDatas[0], nullptr);
	if (nullptr == pBone)
		return E_FAIL;

	m_vecBones.push_back(pBone);

	for (_uint i = 1; i < ModelData.iNumBones; ++i)
	{
		CBone* pBone = CBone::Create(ModelData.pBoneDatas[i], m_vecBones[ModelData.pBoneDatas[i].iParentIdx]);
		if (nullptr == pBone)
			return E_FAIL;

		m_vecBones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(const MODEL_BINARYDATA& ModelData)
{
	m_iNumAnimations = ModelData.iNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(ModelData.pAnimations[i]);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_vecAnimations.push_back(pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
	TYPE eModelType, const MODEL_BINARYDATA& ModelData)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, ModelData)))
	{
		MSG_BOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CModel* CModel::Clone(CComponent * pOwner, void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	for (auto& pMesh : m_vecMeshes)
		Safe_Release(pMesh);
	m_vecMeshes.clear();

	for (auto& Meterial : m_vecMaterials)
	{
		for (auto& Texture : Meterial.pMtrlTexture)
			Safe_Release(Texture);
	}
	m_vecMaterials.clear();

	for (auto& pBone : m_vecBones)
		Safe_Release(pBone);

	m_vecBones.clear();

	CComponent::Free();
}
