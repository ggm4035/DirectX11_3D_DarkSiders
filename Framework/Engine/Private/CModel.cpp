#include "CModel.h"
#include "CMesh.h"
#include "CBone.h"

#include "CGameInstance.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_pModelDataDesc(rhs.m_pModelDataDesc)	// 이거 위험한 얕은 복사임 나중에 꼭 수정할것
	, m_vecMeshes(rhs.m_vecMeshes)
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

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const NONANIM_MODEL_BINARYDATA& ModelData, _fmatrix PivotMatrix)
{
	string FilePath = CGameInstance::GetInstance()->wstrToStr(ModelData.szFilePath);

	m_pModelDataDesc = &ModelData;

	_uint iFlag = 0;

	if (TYPE_NONANIM == eModelType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(FilePath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(eModelType, PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(CComponent* pOwner, void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(const _uint& iMeshIndex)
{
	if (iMeshIndex < 0 || iMeshIndex >= m_pModelDataDesc->iNumMeshes)
		return E_FAIL;

	return m_vecMeshes[iMeshIndex]->Render();
}

void CModel::Play_Animation()
{
	for (auto& pBone : m_vecBones)
		pBone->Invalidate_CombinedTransformationMatrix();
}

HRESULT CModel::Bind_Material(CShader* pShader, const string& strTypename, const _uint& iMeshIndex, TEXTURETYPE eTextureType)
{
	if (nullptr == pShader ||
		iMeshIndex < 0 || iMeshIndex >= m_pModelDataDesc->iNumMeshes ||
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
		iMeshIndex < 0 || iMeshIndex >= m_pModelDataDesc->iNumMeshes)
		return E_FAIL;

	_float4x4 BoneMatrices[256];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 256);

	m_vecMeshes[iMeshIndex]->Get_Matrices(m_vecBones, BoneMatrices);

	return pShader->Bind_Float4x4_Array(strTypename, BoneMatrices, 256);
}

HRESULT CModel::Ready_Meshes(TYPE eModelType, _fmatrix PivotMatrix)
{
	for (_uint i = 0; i < m_pModelDataDesc->iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eModelType, m_vecBones, m_pModelDataDesc->pMeshData[i], PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_vecMeshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials()
{
	for (_uint i = 0; i < m_pModelDataDesc->iNumMeshes; ++i)
	{
		MESHMATERIAL MeshMaterialDesc;
		ZeroMemory(&MeshMaterialDesc, sizeof MeshMaterialDesc);
		
		for (_uint j = 0; j < TEXTURE_TYPE_MAX; ++j)
		{
			if (0 == lstrlen(m_pModelDataDesc->pMeshData[i].szMaterialTexturePath[j]))
				continue;

			MeshMaterialDesc.pMtrlTexture[j] = CTexture::Create(m_pDevice, m_pContext, m_pModelDataDesc->pMeshData[i].szMaterialTexturePath[j]);
			if (nullptr == MeshMaterialDesc.pMtrlTexture[j])
				return E_FAIL;
		}

		m_vecMaterials.push_back(MeshMaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAINode, class CBone* pParent)
{
	CBone* pBone = CBone::Create(pAINode, pParent);
	if (nullptr == pBone)
		return E_FAIL;

	m_vecBones.push_back(pBone);

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
		Ready_Bones(pAINode->mChildren[i], pBone);

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
	TYPE eModelType, const NONANIM_MODEL_BINARYDATA& ModelData, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, ModelData, PivotMatrix)))
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
