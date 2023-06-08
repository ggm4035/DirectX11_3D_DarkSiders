#include "CModel.h"
#include "CMesh.h"

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
{
	/* 지금 얕은복사 한거고 나중에 없앨 수도 있음(릴리즈도 마찬가지) */
	for (auto& pMesh : m_vecMeshes)
		Safe_AddRef(pMesh);

	for (auto& Meterial : m_vecMaterials)
	{
		for (auto& Texture : Meterial.pMtrlTexture)
			Safe_AddRef(Texture);
	}
}

HRESULT CModel::Initialize_Prototype(const wstring& wstrFilePath, _fmatrix PivotMatrix)
{
	string FilePath = CGameInstance::GetInstance()->wstrToStr(wstrFilePath);

	m_pAIScene = m_Importer.ReadFile(FilePath, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMeshes = m_pAIScene->mNumMeshes;

	if (FAILED(Ready_Meshes(PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(wstrFilePath)))
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

HRESULT CModel::Bind_Material(CShader* pShader, const string strTypename, const _uint& iMeshIndex, aiTextureType eTextureType)
{
	if (nullptr == pShader ||
		iMeshIndex < 0 || iMeshIndex >= m_iNumMeshes ||
		eTextureType < 0 || eTextureType >= AI_TEXTURE_TYPE_MAX)
		return E_FAIL;

	return m_vecMaterials[iMeshIndex].pMtrlTexture[eTextureType]->Bind_ShaderResource(pShader, strTypename);
}

HRESULT CModel::Ready_Meshes(_fmatrix PivotMatrix)
{
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i], PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_vecMeshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const wstring& wstrFilePath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_iNumMaterials = m_pAIScene->mNumMaterials;
	string strFilePath = pGameInstance->wstrToStr(wstrFilePath);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MESHMATERIAL MeshMaterialDesc;
		ZeroMemory(&MeshMaterialDesc, sizeof MeshMaterialDesc);

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString strPath;

			if (FAILED(m_pAIScene->mMaterials[i]->GetTexture((aiTextureType)j, 0, &strPath)))
				continue;

			_char szName[MAX_PATH] = { "" };
			_char szExt[MAX_PATH] = { "" };

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szName, MAX_PATH, szExt, MAX_PATH);

			_char szDrive[MAX_PATH] = { "" };
			_char szDirectory[MAX_PATH] = { "" };

			_splitpath_s(strFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			string strFullPath;
			strFullPath = strFullPath + szDrive + szDirectory + szName + szExt;

			MeshMaterialDesc.pMtrlTexture[j] = CTexture::Create(m_pDevice, m_pContext, pGameInstance->strToWStr(strFullPath));
			if (nullptr == MeshMaterialDesc.pMtrlTexture[j])
				return E_FAIL;
		}

		m_vecMaterials.push_back(MeshMaterialDesc);
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& wstrFilePath, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrFilePath, PivotMatrix)))
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

	CComponent::Free();
}
