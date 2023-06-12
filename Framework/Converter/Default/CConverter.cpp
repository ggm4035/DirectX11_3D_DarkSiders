#include "CConverter.h"

#include "CBone.h"
#include "CGameInstance.h"

CConverter::CConverter()
{
}

HRESULT CConverter::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

void CConverter::Tick()
{
	/* NonAnimModel 바이너리화 */
	m_pGameInstance->Extraction_Data("../Bin/Resources/NonAnimModels/Environment/BreakAble", ".fbx", m_FilePathList);
	ConvertBinary_NonAnimModel();
	WriteModels("NonAnimModelsFile.dat");
	//ResetData();
	//m_pGameInstance->ReadModels("NonAnimModelsFile.dat", m_FilePathList, m_vecDatas); /* 잘 읽는지 체크용 */

	ResetData();

	/* AnimModel 바이너리화 */
	m_pGameInstance->Extraction_Data("../Bin/Resources/Models/ForkLift", ".fbx", m_FilePathList);
	ConvertBinary_AnimModel();
	WriteModels("ForkLift.dat");
	//ResetData();"../Bin/Resources/AnimModels/Heros/Warrior"
	//m_pGameInstance->ReadModels("AnimModelsFile.dat", m_FilePathList, m_vecDatas); /* 잘 읽는지 체크용 */
}

void CConverter::WriteModels(const string& strFileName)
{
	_char szFullPath[MAX_PATH] = { "" };
	GetCurrentDirectoryA(MAX_PATH, szFullPath);
	string strFullPath = szFullPath;

	strFullPath += "/" + strFileName;

	HANDLE hFile = CreateFileA(strFullPath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	_ulong dwByte = { 0 };

	auto strPath = m_FilePathList.begin();

	for (_uint iDataIndex = 0; iDataIndex < m_FilePathList.size(); ++iDataIndex)
	{
		/* Write strFilePath */
		_uint iPathLength = strlen(strPath->c_str()) + 1;
		WriteFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, strPath->c_str(), sizeof(_char) * iPathLength, &dwByte, nullptr);
		++strPath;

		/*========== BINARY DATAS ===========*/
		/* Write szFilePath*/
		iPathLength = lstrlen(m_vecDatas[iDataIndex].szFilePath) + 1;
		WriteFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, m_vecDatas[iDataIndex].szFilePath, sizeof(_tchar) * iPathLength, &dwByte, nullptr);

		/* Write szTag */
		iPathLength = lstrlen(m_vecDatas[iDataIndex].szTag) + 1;
		WriteFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, m_vecDatas[iDataIndex].szTag, sizeof(_tchar) * iPathLength, &dwByte, nullptr);

		/* Write iNumMeshes */
		WriteFile(hFile, &m_vecDatas[iDataIndex].iNumMeshes, sizeof(_uint), &dwByte, nullptr);

		/* Write iNumMaterials */
		WriteFile(hFile, &m_vecDatas[iDataIndex].iNumMaterials, sizeof(_uint), &dwByte, nullptr);

		/* Write szMaterialTexturePath */
		_uint iTexturePathlength = { 0 };
		for (_uint iMaterialIndex = 0; iMaterialIndex < m_vecDatas[iDataIndex].iNumMaterials; ++iMaterialIndex)
		{
			for (_uint iTextureIndex = 0; iTextureIndex < 21; ++iTextureIndex)
			{
				iTexturePathlength = lstrlen(m_vecDatas[iDataIndex].pMaterialPaths[iMaterialIndex].szMaterialTexturePath[iTextureIndex]) + 1;
				WriteFile(hFile, &iTexturePathlength, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, m_vecDatas[iDataIndex].pMaterialPaths[iMaterialIndex].szMaterialTexturePath[iTextureIndex],
					sizeof(_tchar) * iTexturePathlength, &dwByte, nullptr);
			}
		}

		/* Write iNumBones */
		WriteFile(hFile, &m_vecDatas[iDataIndex].iNumBones, sizeof(_uint), &dwByte, nullptr);

		/*========== BONE DATAS ===========*/
		for (const auto& Bone : m_vecBones)
		{
			/* Write szName_Bone */
			_uint iNameLength = strlen(Bone.szName) + 1;
			WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, Bone.szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Write iParentIdx */
			WriteFile(hFile, &Bone.iParentIdx, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumChildren */
			WriteFile(hFile, &Bone.iNumChildren, sizeof(_uint), &dwByte, nullptr);

			/* Write TransformationMatrix */
			WriteFile(hFile, &Bone.TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

			/* Write OffsetMatrix */
			WriteFile(hFile, &Bone.OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);
		}

		/*========== MESH DATAS ===========*/
		for (_uint iMeshIndex = 0; iMeshIndex < m_vecDatas[iDataIndex].iNumMeshes; ++iMeshIndex)
		{
			/* Write szName_Mesh */
			_uint iNameLength = strlen(m_vecDatas[iDataIndex].pMeshData[iMeshIndex].szName) + 1;
			WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshData[iMeshIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Write iMaterialIndex */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumNonAnimVertices */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumNonAnimVertices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumAnimVertices */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumAnimVertices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumIndices */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumIndices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumMeshBones */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumMeshBones, sizeof(_uint), &dwByte, nullptr);

			/* Write pBoneIndices */
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshData[iMeshIndex].pBoneIndices, sizeof(_uint) * 
				m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumMeshBones, &dwByte, nullptr);

			/* Write pNonAnimVertices */
			if (0 < m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumNonAnimVertices)
			{
				WriteFile(hFile, m_vecDatas[iDataIndex].pMeshData[iMeshIndex].pNonAnimVertices, sizeof(VTXMESH) *
					m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumNonAnimVertices, &dwByte, nullptr);
			}

			/* Write pAnimVertices */
			if (0 < m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumAnimVertices)
			{
				WriteFile(hFile, m_vecDatas[iDataIndex].pMeshData[iMeshIndex].pAnimVertices, sizeof(VTXANIMMESH) *
					m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumAnimVertices, &dwByte, nullptr);
			}

			/* Write pIndices */
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshData[iMeshIndex].pIndices, sizeof(_ulong) *
				m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumIndices, &dwByte, nullptr);
		}
	}

	CloseHandle(hFile);
}

void CConverter::ConvertBinary_AnimModel()
{
	m_vecDatas.reserve(m_FilePathList.size());

	_uint iIndex = 0;
	for (auto& Path : m_FilePathList)
	{
		MODEL_BINARYDATA BinDataDesc;
		ZeroMemory(&BinDataDesc, sizeof BinDataDesc);
		
		m_pAIScene = m_Importer.ReadFile(Path, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

		if (nullptr == m_pAIScene)
			return;

		lstrcpy(BinDataDesc.szFilePath, CGameInstance::GetInstance()->strToWStr(Path).c_str());

		_tchar szTag[MAX_PATH] = { L"" };

		_wsplitpath_s(BinDataDesc.szFilePath, nullptr, 0, nullptr, 0, szTag, MAX_PATH, nullptr, 0);

		lstrcpy(BinDataDesc.szTag, szTag);

		/* Bone Setting */
		if (FAILED(Ready_Bones(m_pAIScene->mRootNode, 0)))
		{
			MSG_BOX("Failed_Ready_AnimBones");
			return;
		}

		BinDataDesc.iNumBones = m_vecBones.size();

		m_iNumMeshes = m_pAIScene->mNumMeshes;
		BinDataDesc.iNumMeshes = m_iNumMeshes;

		BinDataDesc.pMeshData = new MESHDATA[m_iNumMeshes];
		ZeroMemory(BinDataDesc.pMeshData, sizeof(MESHDATA) * m_iNumMeshes);

		/* Mesh Setting */
		if (FAILED(Ready_Meshes_AnimModel(BinDataDesc.pMeshData)))
		{
			MSG_BOX("Failed_Ready_AnimMeshes");
			return;
		}

		BinDataDesc.iNumMaterials = m_pAIScene->mNumMaterials;

		/* Material Setting */
		if (FAILED(Ready_Materials(Path, BinDataDesc)))
		{
			MSG_BOX("Failed_Ready_AnimMateriala");
			return;
		}

		m_vecDatas.push_back(BinDataDesc);
		++iIndex;
	}
}

void CConverter::ConvertBinary_NonAnimModel()
{
	m_vecDatas.reserve(m_FilePathList.size());

	_uint iIndex = 0;
	for (auto& Path : m_FilePathList)
	{
		MODEL_BINARYDATA BinDataDesc;
		ZeroMemory(&BinDataDesc, sizeof BinDataDesc);

		m_pAIScene = m_Importer.ReadFile(Path, aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
		
		if (nullptr == m_pAIScene)
			return;

		lstrcpy(BinDataDesc.szFilePath, CGameInstance::GetInstance()->strToWStr(Path).c_str());

		_tchar szTag[MAX_PATH] = { L"" };

		_wsplitpath_s(BinDataDesc.szFilePath, nullptr, 0, nullptr, 0, szTag, MAX_PATH, nullptr, 0);

		lstrcpy(BinDataDesc.szTag, szTag);

		m_iNumMeshes = m_pAIScene->mNumMeshes;
		BinDataDesc.iNumMeshes = m_iNumMeshes;

		_matrix PivotMatrix = XMMatrixIdentity();
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixRotationZ(XMConvertToRadians(-90.f));
		BinDataDesc.pMeshData = new MESHDATA[m_iNumMeshes];
		ZeroMemory(BinDataDesc.pMeshData, sizeof(MESHDATA) * m_iNumMeshes);

		if (FAILED(Ready_Meshes_NonAnimModel(PivotMatrix, BinDataDesc.pMeshData)))
		{
			MSG_BOX("Failed_Ready_NonAnimMeshes");
			return;
		}

		BinDataDesc.iNumMaterials = m_pAIScene->mNumMaterials;
		if (FAILED(Ready_Materials(Path, BinDataDesc)))
		{
			MSG_BOX("Failed_Ready_NonAnimMateriala");
			return;
		}
		m_vecDatas.push_back(BinDataDesc);
		++iIndex;
	}
}

HRESULT CConverter::Ready_Bones(aiNode* pAINode, const _uint& iParentIdx)
{
	static _uint iIndex = { 0 };
	BONEDATA BoneData;
	ZeroMemory(&BoneData, sizeof BoneData);

	strcpy_s(BoneData.szName, pAINode->mName.data);
	memcpy(&BoneData.TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&BoneData.TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&BoneData.TransformationMatrix)));
	BoneData.iNumChildren = pAINode->mNumChildren;
	BoneData.iParentIdx = iParentIdx;

	_uint data = iIndex;
	m_vecBones.push_back(BoneData);
	++iIndex;

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
		Ready_Bones(pAINode->mChildren[i], data);

	return S_OK;
}

HRESULT CConverter::Ready_Meshes_AnimModel(OUT MESHDATA* pData)
{
	for (_uint iMeshIndex = 0; iMeshIndex < m_iNumMeshes; ++iMeshIndex)
	{
		const aiMesh* pAIMesh = m_pAIScene->mMeshes[iMeshIndex];

		m_strName = pAIMesh->mName.data;
		strcpy_s(pData[iMeshIndex].szName, m_strName.c_str());

		m_iMaterialIndex = pAIMesh->mMaterialIndex;
		pData[iMeshIndex].iMaterialIndex = m_iMaterialIndex;

		m_iNumVertices = { pAIMesh->mNumVertices };
		pData[iMeshIndex].iNumAnimVertices = m_iNumVertices;

		m_iNumIndices = { pAIMesh->mNumFaces * 3 };
		pData[iMeshIndex].iNumIndices = m_iNumIndices;

		pData[iMeshIndex].pAnimVertices = new VTXANIMMESH[m_iNumVertices];
		ZeroMemory(pData[iMeshIndex].pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

		for (_uint iVtxIndex = 0; iVtxIndex < m_iNumVertices; ++iVtxIndex)
		{
			memcpy(&pData[iMeshIndex].pAnimVertices[iVtxIndex].vPosition, &pAIMesh->mVertices[iVtxIndex], sizeof(_float3));
			memcpy(&pData[iMeshIndex].pAnimVertices[iVtxIndex].vNormal, &pAIMesh->mNormals[iVtxIndex], sizeof(_float3));
			memcpy(&pData[iMeshIndex].pAnimVertices[iVtxIndex].vTexCoord, &pAIMesh->mTextureCoords[0][iVtxIndex], sizeof(_float2));
			memcpy(&pData[iMeshIndex].pAnimVertices[iVtxIndex].vTangent, &pAIMesh->mTangents[iVtxIndex], sizeof(_float3));
		}

		pData[iMeshIndex].iNumMeshBones = pAIMesh->mNumBones;

		if (0 < pData[iMeshIndex].iNumMeshBones)
		{
			pData[iMeshIndex].pBoneIndices = new _uint[pData[iMeshIndex].iNumMeshBones];
			ZeroMemory(pData[iMeshIndex].pBoneIndices, sizeof(_uint) * pData[iMeshIndex].iNumMeshBones);
		}

		/* pAIMesh->mNumBones : 이 메시는 몇개의 뼈의 정보를 사용하는가. */
		for (_uint i = 0; i < pAIMesh->mNumBones; ++i)
		{
			aiBone* pAIBone = pAIMesh->mBones[i];

			_uint iBoneIndex = { 0 };

			for (auto& BoneData : m_vecBones)
			{
				if (0 != strcmp(pAIBone->mName.data, BoneData.szName))
					++iBoneIndex;
				else
					break;
			}

			_float4x4 OffsetMatrix;
			ZeroMemory(&OffsetMatrix, sizeof OffsetMatrix);
			memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

			XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
			m_vecBones[iBoneIndex].OffsetMatrix = OffsetMatrix;

			pData[iMeshIndex].pBoneIndices[i] = iBoneIndex;

			/* pAIBone->mNumWeights : 이뼈는 몇개 정점에게 영향을 주니. */
			for (_uint iWeightIndex = 0; iWeightIndex < pAIBone->mNumWeights; iWeightIndex++)
			{
				/* AIVertexWeight : iBoneIndex번째 정점에게 영향을 주는정보. */
				aiVertexWeight	AIVertexWeight = pAIBone->mWeights[iWeightIndex];

				if (0.0f == pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendWeights.x)
				{
					pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendIndices.x = i;
					pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendWeights.x = AIVertexWeight.mWeight;
				}

				else if (0.0f == pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendWeights.y)
				{
					pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendIndices.y = i;
					pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendWeights.y = AIVertexWeight.mWeight;
				}

				else if (0.0f == pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendWeights.z)
				{
					pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendIndices.z = i;
					pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendWeights.z = AIVertexWeight.mWeight;
				}

				else if (0.0f == pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendWeights.w)
				{
					pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendIndices.w = i;
					pData[iMeshIndex].pAnimVertices[AIVertexWeight.mVertexId].vBlendWeights.w = AIVertexWeight.mWeight;
				}
			}
		}

		_uint iNumIndices = { 0 };
		pData[iMeshIndex].pIndices = new _ulong[m_iNumIndices];

		for (_uint j = 0; j < pAIMesh->mNumFaces; ++j)
		{
			pData[iMeshIndex].pIndices[iNumIndices++] = pAIMesh->mFaces[j].mIndices[0];
			pData[iMeshIndex].pIndices[iNumIndices++] = pAIMesh->mFaces[j].mIndices[1];
			pData[iMeshIndex].pIndices[iNumIndices++] = pAIMesh->mFaces[j].mIndices[2];
		}
	}

	return S_OK;
}

HRESULT CConverter::Ready_Meshes_NonAnimModel(_fmatrix PivotMatrix, OUT MESHDATA* pData)
{
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		const aiMesh* pAIMesh = m_pAIScene->mMeshes[i];

		m_strName = pAIMesh->mName.data;
		strcpy_s(pData[i].szName, m_strName.c_str());

		m_iMaterialIndex = pAIMesh->mMaterialIndex;
		pData[i].iMaterialIndex = m_iMaterialIndex;

		m_iNumVertices = { pAIMesh->mNumVertices };
		pData[i].iNumNonAnimVertices = m_iNumVertices;

		m_iNumIndices = { pAIMesh->mNumFaces * 3 };
		pData[i].iNumIndices = m_iNumIndices;

		pData[i].pNonAnimVertices = new VTXMESH[m_iNumVertices];

		for (_uint j = 0; j < m_iNumVertices; ++j)
		{
			memcpy(&pData[i].pNonAnimVertices[j].vPosition, &pAIMesh->mVertices[j], sizeof(_float3));
			XMStoreFloat3(&pData[i].pNonAnimVertices[j].vPosition,
				XMVector3TransformCoord(XMLoadFloat3(&pData[i].pNonAnimVertices[j].vPosition), PivotMatrix));

			memcpy(&pData[i].pNonAnimVertices[j].vNormal, &pAIMesh->mNormals[j], sizeof(_float3));
			XMStoreFloat3(&pData[i].pNonAnimVertices[j].vNormal,
				XMVector3TransformCoord(XMLoadFloat3(&pData[i].pNonAnimVertices[j].vNormal), PivotMatrix));

			memcpy(&pData[i].pNonAnimVertices[j].vTexCoord, &pAIMesh->mTextureCoords[0][j], sizeof(_float2));
			memcpy(&pData[i].pNonAnimVertices[j].vTangent, &pAIMesh->mTangents[j], sizeof(_float3));
		}

		_uint iNumIndices = { 0 };
		pData[i].pIndices = new _ulong[m_iNumIndices];

		for (_uint j = 0; j < pAIMesh->mNumFaces; ++j)
		{
			pData[i].pIndices[iNumIndices++] = pAIMesh->mFaces[j].mIndices[0];
			pData[i].pIndices[iNumIndices++] = pAIMesh->mFaces[j].mIndices[1];
			pData[i].pIndices[iNumIndices++] = pAIMesh->mFaces[j].mIndices[2];
		}
	}
	return S_OK;
}

HRESULT CConverter::Ready_Materials(const string& strFilePath, OUT MODEL_BINARYDATA& Data)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	if (0 < m_iNumMaterials)
	{
		Data.pMaterialPaths = new MATERIALPATH[m_iNumMaterials];
		ZeroMemory(Data.pMaterialPaths, sizeof(MATERIALPATH) * m_iNumMaterials);
	}

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
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

			lstrcpy(Data.pMaterialPaths[i].szMaterialTexturePath[j], m_pGameInstance->strToWStr(strFullPath).c_str());
		}
	}

	return S_OK;
}

CConverter* CConverter::Create()
{
	CConverter* pInstance = new CConverter;

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Create Converter");
	}
	return pInstance;
}

void CConverter::ResetData()
{
	m_FilePathList.clear();

	for (auto& Model : m_vecDatas)
	{
		for (_uint i = 0; i < Model.iNumMeshes; ++i)
		{
			Safe_Delete_Array(Model.pMeshData[i].pBoneIndices);
			Safe_Delete_Array(Model.pMeshData[i].pNonAnimVertices);
			Safe_Delete_Array(Model.pMeshData[i].pAnimVertices);
			Safe_Delete_Array(Model.pMeshData[i].pIndices);
		}
		Safe_Delete_Array(Model.pMaterialPaths);
		Safe_Delete_Array(Model.pMeshData);
		Safe_Delete_Array(Model.pBoneDatas);
	}
	m_vecDatas.clear();
	m_vecBones.clear();
}

void CConverter::Free()
{
	ResetData();
	Safe_Release(m_pGameInstance);
}
