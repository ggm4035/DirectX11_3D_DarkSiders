#include "CModelLoader.h"

#include "CGameInstance.h"

CModelLoader::CModelLoader()
{
}

CModelLoader::~CModelLoader()
{
	Free();
}

HRESULT CModelLoader::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

void CModelLoader::Tick()
{
	/* NonAnimModel 바이너리화 */
	m_pGameInstance->Extraction_Data("../../Resources/NonAnimModels/Environment/Hell", ".fbx", m_FilePathList);
	ConvertBinary_NonAnimModel();
	WriteNonAnimModels("../../ModelDatas/NonAnimModels/");

	ResetData();

	/* Weapon 바이너리화 */
	m_pGameInstance->Extraction_Data("../../Resources/NonAnimModels/Weapon", ".fbx", m_FilePathList);
	ConvertBinary_AnimModel();
	WriteAnimModels("../../ModelDatas/Weapon/");

	ResetData();

	/* AnimModel 바이너리화 */
	m_pGameInstance->Extraction_Data("../../Resources/AnimModels", ".fbx", m_FilePathList);
	ConvertBinary_AnimModel();
	WriteAnimModels("../../ModelDatas/AnimModels/");
}

void CModelLoader::WriteAnimModels(const string& strFilePath)
{
	auto strPath = m_FilePathList.begin();

	HANDLE hFile = 0;

	for (_uint iDataIndex = 0; iDataIndex < m_FilePathList.size(); ++iDataIndex)
	{
		string strFullPath = strFilePath;
		_char szFileName[MAX_PATH] = { "" };
		_splitpath_s(strPath->c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		strFullPath = strFullPath + szFileName + ".dat";

		hFile = CreateFileA(strFullPath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		std::cout << "==== Write Binary Datas " << szFileName << " ====" << endl;
		_ulong dwByte = { 0 };

		/* Write strFilePath */
		_uint iPathLength = strlen(strPath->c_str()) + 1;
		WriteFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, strPath->c_str(), sizeof(_char) * iPathLength, &dwByte, nullptr);
		++strPath;

		/*===== BINARY DATAS ======*/
		std::cout << "WriteFile Binary Datas..." << endl;
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

		/*===== BONE DATAS ======*/
		std::cout << "WriteFile Bone Datas..." << endl;
		if (0 < m_vecBones.size())
		{
			for (auto& Bones : m_vecBones[iDataIndex])
			{
				/* Write szName_Bone */
				_uint iNameLength = strlen(Bones.szName) + 1;
				WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, Bones.szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

				/* Write iParentIdx */
				WriteFile(hFile, &Bones.iParentIdx, sizeof(_uint), &dwByte, nullptr);

				/* Write iIndex */
				WriteFile(hFile, &Bones.iIndex, sizeof(_uint), &dwByte, nullptr);

				/* Write TransformationMatrix */
				WriteFile(hFile, &Bones.TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

				/* Write OffsetMatrix */
				WriteFile(hFile, &Bones.OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);
			}
		}

		/* Write iNumAnimations */
		WriteFile(hFile, &m_vecDatas[iDataIndex].iNumAnimations, sizeof(_uint), &dwByte, nullptr);

		/*===== ANIMATION DATAS ======*/
		std::cout << "WriteFile Animation Datas..." << endl;
		for (auto& AnimationData : m_vecDatas[iDataIndex].vecAnimations)
		{
			/* Write szName */
			_uint iNameLength = strlen(AnimationData.szName) + 1;
			WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, AnimationData.szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Write Duration */
			WriteFile(hFile, &AnimationData.Duration, sizeof(_double), &dwByte, nullptr);

			/* Write TickPerSec */
			WriteFile(hFile, &AnimationData.TickPerSec, sizeof(_double), &dwByte, nullptr);

			/* Write iNumChannels */
			WriteFile(hFile, &AnimationData.iNumChannels, sizeof(_uint), &dwByte, nullptr);

			/* Write bIsLoop */
			WriteFile(hFile, &AnimationData.bIsLoop, sizeof(_bool), &dwByte, nullptr);

			/* Write bIsFollowAnimation */
			WriteFile(hFile, &AnimationData.bIsFollowAnimation, sizeof(_bool), &dwByte, nullptr);

			std::cout << "WriteFile Channel Datas..." << endl;
			/*===== CHANNEL DATAS ======*/
			for (auto& ChannelData : AnimationData.vecChannels)
			{
				/* Write szName */
				iNameLength = strlen(ChannelData.szName) + 1;
				WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, ChannelData.szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

				/* Write iNumKeyFrames */
				WriteFile(hFile, &ChannelData.iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);

				/* Write KeyFrame */
				for (auto& KeyFrameData : ChannelData.vecKeyFrames)
					WriteFile(hFile, &KeyFrameData, sizeof(KEYFRAME), &dwByte, nullptr);
			}
		}

		/*===== MESH DATAS ======*/
		std::cout << "WriteFile Mesh Datas..." << endl;
		for (_uint iMeshIndex = 0; iMeshIndex < m_vecDatas[iDataIndex].iNumMeshes; ++iMeshIndex)
		{
			/* Write szName_Mesh */
			_uint iNameLength = strlen(m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].szName) + 1;
			WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Write iMaterialIndex */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumNonAnimVertices */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumNonAnimVertices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumAnimVertices */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumAnimVertices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumIndices */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumIndices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumMeshBones */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumMeshBones, sizeof(_uint), &dwByte, nullptr);

			/* Write pBoneIndices */
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].pBoneIndices, sizeof(_uint) *
				m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumMeshBones, &dwByte, nullptr);

			/* Write pNonAnimVertices */
			if (0 < m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumNonAnimVertices)
			{
				WriteFile(hFile, m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].pNonAnimVertices, sizeof(VTXMESH) *
					m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumNonAnimVertices, &dwByte, nullptr);
			}

			/* Write pAnimVertices */
			if (0 < m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumAnimVertices)
			{
				WriteFile(hFile, m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].pAnimVertices, sizeof(VTXANIMMESH) *
					m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumAnimVertices, &dwByte, nullptr);
			}

			/* Write pIndices */
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].pIndices, sizeof(_ulong) *
				m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumIndices, &dwByte, nullptr);
		}

		CloseHandle(hFile);
		std::cout << "==== Success Write Binary Datas ====" << endl;
	}
}

void CModelLoader::WriteNonAnimModels(const string& strFilePath)
{
	auto strPath = m_FilePathList.begin();
	
	for (_uint iDataIndex = 0; iDataIndex < m_FilePathList.size(); ++iDataIndex)
	{
		string strFullPath = strFilePath;
		_char szFileName[MAX_PATH] = { "" };
		_splitpath_s(strPath->c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		strFullPath = strFullPath + szFileName + ".dat";

		HANDLE hFile = CreateFileA(strFullPath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		std::cout << "==== Write Binary Datas " << szFileName << " ====" << endl;
		_ulong dwByte = { 0 };

		/* Write strFilePath */
		_uint iPathLength = strlen(strPath->c_str()) + 1;
		WriteFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, strPath->c_str(), sizeof(_char) * iPathLength, &dwByte, nullptr);
		++strPath;

		/*===== BINARY DATAS ======*/
		std::cout << "WriteFile Binary Datas..." << endl;
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

		/*===== BONE DATAS ======*/
		std::cout << "WriteFile Bone Datas..." << endl;
		if (0 < m_vecBones.size())
		{
			for (auto& Bones : m_vecBones[iDataIndex])
			{
				/* Write szName_Bone */
				_uint iNameLength = strlen(Bones.szName) + 1;
				WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, Bones.szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

				/* Write iParentIdx */
				WriteFile(hFile, &Bones.iParentIdx, sizeof(_uint), &dwByte, nullptr);

				/* Write iIndex */
				WriteFile(hFile, &Bones.iIndex, sizeof(_uint), &dwByte, nullptr);

				/* Write TransformationMatrix */
				WriteFile(hFile, &Bones.TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

				/* Write OffsetMatrix */
				WriteFile(hFile, &Bones.OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);
			}
		}

		/* Write iNumAnimations */
		WriteFile(hFile, &m_vecDatas[iDataIndex].iNumAnimations, sizeof(_uint), &dwByte, nullptr);

		/*===== ANIMATION DATAS ======*/
		std::cout << "WriteFile Animation Datas..." << endl;
		for (auto& AnimationData : m_vecDatas[iDataIndex].vecAnimations)
		{
			/* Write szName */
			_uint iNameLength = strlen(AnimationData.szName) + 1;
			WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, AnimationData.szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Write Duration */
			WriteFile(hFile, &AnimationData.Duration, sizeof(_double), &dwByte, nullptr);

			/* Write TickPerSec */
			WriteFile(hFile, &AnimationData.TickPerSec, sizeof(_double), &dwByte, nullptr);

			/* Write iNumChannels */
			WriteFile(hFile, &AnimationData.iNumChannels, sizeof(_uint), &dwByte, nullptr);

			/* Write bIsLoop */
			WriteFile(hFile, &AnimationData.bIsLoop, sizeof(_bool), &dwByte, nullptr);
		}

		/*===== MESH DATAS ======*/
		std::cout << "WriteFile Mesh Datas..." << endl;
		for (_uint iMeshIndex = 0; iMeshIndex < m_vecDatas[iDataIndex].iNumMeshes; ++iMeshIndex)
		{
			/* Write szName_Mesh */
			_uint iNameLength = strlen(m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].szName) + 1;
			WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Write iMaterialIndex */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumNonAnimVertices */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumNonAnimVertices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumAnimVertices */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumAnimVertices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumIndices */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumIndices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumMeshBones */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumMeshBones, sizeof(_uint), &dwByte, nullptr);

			/* Write pBoneIndices */
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].pBoneIndices, sizeof(_uint) *
				m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumMeshBones, &dwByte, nullptr);

			/* Write pNonAnimVertices */
			if (0 < m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumNonAnimVertices)
			{
				WriteFile(hFile, m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].pNonAnimVertices, sizeof(VTXMESH) *
					m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumNonAnimVertices, &dwByte, nullptr);
			}

			/* Write pAnimVertices */
			if (0 < m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumAnimVertices)
			{
				WriteFile(hFile, m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].pAnimVertices, sizeof(VTXANIMMESH) *
					m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumAnimVertices, &dwByte, nullptr);
			}

			/* Write pIndices */
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].pIndices, sizeof(_ulong) *
				m_vecDatas[iDataIndex].pMeshDatas[iMeshIndex].iNumIndices, &dwByte, nullptr);
		}
		CloseHandle(hFile);
		std::cout << "==== Success Write Binary Datas ====" << endl;
	}
}

HRESULT CModelLoader::ConvertBinary_AnimModel()
{
	m_vecDatas.reserve(m_FilePathList.size());

	_uint iIndex = 0;
	for (auto& Path : m_FilePathList)
	{
		vector<BONEDATA> Bones;
		_char szFileName[MAX_PATH] = { "" };
		_splitpath_s(Path.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		std::cout << "==== Convert Binary AnimModels Datas " << szFileName << " ====" << endl;
		MODEL_BINARYDATA BinDataDesc;
		ZeroMemory(&BinDataDesc, sizeof BinDataDesc);

		m_pAIScene = m_Importer.ReadFile(Path, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

		if (nullptr == m_pAIScene)
			return E_FAIL;

		_tchar szFilePath[MAX_PATH] = { L"" };
		lstrcpy(szFilePath, CGameInstance::GetInstance()->strToWStr(Path).c_str());

		_tchar szTag[MAX_PATH] = { L"" };

		_wsplitpath_s(szFilePath, nullptr, 0, nullptr, 0, szTag, MAX_PATH, nullptr, 0);

		lstrcpy(BinDataDesc.szTag, szTag);

		/* Bone Setting */
		m_iBoneIndex = 0;
		std::cout << "Convert Binary Bones..." << endl;
		if (FAILED(Ready_Bones(m_pAIScene->mRootNode, Bones, m_iBoneIndex)))
		{
			MSG_BOX("Failed_Ready_AnimBones");
			return E_FAIL;
		}

		BinDataDesc.iNumBones = Bones.size();

		m_iNumMeshes = m_pAIScene->mNumMeshes;
		BinDataDesc.iNumMeshes = m_iNumMeshes;

		BinDataDesc.pMeshDatas = new MESHDATA[m_iNumMeshes];
		ZeroMemory(BinDataDesc.pMeshDatas, sizeof(MESHDATA) * m_iNumMeshes);

		/* Mesh Setting */
		std::cout << "Convert Binary Meshes..." << endl;
		if (FAILED(Ready_Meshes_AnimModel(Bones, BinDataDesc.pMeshDatas)))
		{
			MSG_BOX("Failed_Ready_AnimMeshes");
			return E_FAIL;
		}

		/* Material Setting */
		std::cout << "Convert Binary Materials..." << endl;
		if (FAILED(Ready_Materials(Path, BinDataDesc)))
		{
			MSG_BOX("Failed_Ready_AnimMateriala");
			return E_FAIL;
		}

		/* Animation Setting */
		std::cout << "Convert Binary Animations..." << endl;
		if (FAILED(Ready_Animation(BinDataDesc)))
		{
			MSG_BOX("Failed_Ready_Animation");
			return E_FAIL;
		}

		m_vecBones.push_back(Bones);
		m_vecDatas.push_back(BinDataDesc);
		++iIndex;
		std::cout << "==== Success Convert Binary AnimModels Datas ====" << endl;
	}
}

HRESULT CModelLoader::ConvertBinary_NonAnimModel()
{
	m_vecDatas.reserve(m_FilePathList.size());

	_uint iIndex = 0;
	for (auto& Path : m_FilePathList)
	{
		_char szFileName[MAX_PATH] = { "" };
		_splitpath_s(Path.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		std::cout << "==== Convert Binary NonAnimModels Datas " << szFileName << " ====" << endl;
		MODEL_BINARYDATA BinDataDesc;
		ZeroMemory(&BinDataDesc, sizeof BinDataDesc);

		m_pAIScene = m_Importer.ReadFile(Path, aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

		if (nullptr == m_pAIScene)
			return E_FAIL;

		_tchar szFilePath[MAX_PATH] = { L"" };
		lstrcpy(szFilePath, CGameInstance::GetInstance()->strToWStr(Path).c_str());

		_tchar szTag[MAX_PATH] = { L"" };

		_wsplitpath_s(szFilePath, nullptr, 0, nullptr, 0, szTag, MAX_PATH, nullptr, 0);

		lstrcpy(BinDataDesc.szTag, szTag);

		m_iNumMeshes = m_pAIScene->mNumMeshes;
		BinDataDesc.iNumMeshes = m_iNumMeshes;

		_matrix PivotMatrix = XMMatrixIdentity();
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixRotationZ(XMConvertToRadians(-90.f));
		BinDataDesc.pMeshDatas = new MESHDATA[m_iNumMeshes];
		ZeroMemory(BinDataDesc.pMeshDatas, sizeof(MESHDATA) * m_iNumMeshes);

		std::cout << "Convert Binary NonAnimModels..." << endl;
		if (FAILED(Ready_Meshes_NonAnimModel(PivotMatrix, BinDataDesc.pMeshDatas)))
		{
			MSG_BOX("Failed_Ready_NonAnimMeshes");
			return E_FAIL;
		}

		std::cout << "Convert Binary Materials..." << endl;
		if (FAILED(Ready_Materials(Path, BinDataDesc)))
		{
			MSG_BOX("Failed_Ready_NonAnimMaterials");
			return E_FAIL;
		}
		m_vecDatas.push_back(BinDataDesc);
		++iIndex;
		std::cout << "==== Success Convert Binary NonAnimModels Datas ====" << endl;
	}
}

HRESULT CModelLoader::Ready_Bones(aiNode* pAINode, vector<BONEDATA>& Bones, const _uint& iParentIdx)
{
	BONEDATA BoneData;
	ZeroMemory(&BoneData, sizeof BoneData);

	strcpy_s(BoneData.szName, pAINode->mName.data);
	std::memcpy(&BoneData.TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&BoneData.TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&BoneData.TransformationMatrix)));
	BoneData.iParentIdx = iParentIdx;
	BoneData.iIndex = Bones.size();

	_uint data = m_iBoneIndex;
	Bones.push_back(BoneData);
	++m_iBoneIndex;

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
		Ready_Bones(pAINode->mChildren[i], Bones, data);

	return S_OK;
}

HRESULT CModelLoader::Ready_Meshes_AnimModel(vector<BONEDATA>& Bones, OUT MESHDATA* pData)
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
			std::memcpy(&pData[iMeshIndex].pAnimVertices[iVtxIndex].vPosition, &pAIMesh->mVertices[iVtxIndex], sizeof(_float3));
			std::memcpy(&pData[iMeshIndex].pAnimVertices[iVtxIndex].vNormal, &pAIMesh->mNormals[iVtxIndex], sizeof(_float3));
			std::memcpy(&pData[iMeshIndex].pAnimVertices[iVtxIndex].vTexCoord, &pAIMesh->mTextureCoords[0][iVtxIndex], sizeof(_float2));
			std::memcpy(&pData[iMeshIndex].pAnimVertices[iVtxIndex].vTangent, &pAIMesh->mTangents[iVtxIndex], sizeof(_float3));
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

			for (auto& BoneData : Bones)
			{
				if (0 != strcmp(pAIBone->mName.data, BoneData.szName))
					++iBoneIndex;
				else
					break;
			}

			_float4x4 OffsetMatrix;
			ZeroMemory(&OffsetMatrix, sizeof OffsetMatrix);
			std::memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

			XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
			Bones[iBoneIndex].OffsetMatrix = OffsetMatrix;

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

		if (0 == pData[iMeshIndex].iNumMeshBones)
		{
			pData[iMeshIndex].pBoneIndices = new _uint;
			ZeroMemory(pData[iMeshIndex].pBoneIndices, sizeof(_uint));

			pData[iMeshIndex].iNumMeshBones = 1;
			
			_uint iBoneIndex = 0;
			
			for (auto& Data : Bones)
			{
				string SrcName = Data.szName;
				string DstName = pData[iMeshIndex].szName;

				SrcName = SrcName.substr(0, SrcName.find("."));

				if (string::npos == DstName.find(SrcName))
					++iBoneIndex;
				else
					break;
			}

			XMStoreFloat4x4(&Bones[iBoneIndex].OffsetMatrix, XMMatrixIdentity());
			pData[iMeshIndex].pBoneIndices[0] = iBoneIndex;
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

HRESULT CModelLoader::Ready_Meshes_NonAnimModel(_fmatrix PivotMatrix, OUT MESHDATA* pData)
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
			XMStoreFloat3(&pData[i].pNonAnimVertices[j].vTangent,
				XMVector3TransformNormal(XMLoadFloat3(&pData[i].pNonAnimVertices[j].vTangent), PivotMatrix));
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

HRESULT CModelLoader::Ready_Animation(MODEL_BINARYDATA& Data)
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;
	Data.iNumAnimations = m_iNumAnimations;

	vector<ANIMATIONDATA> vecAnimation;
	for (_uint iAnimIndex = 0; iAnimIndex < m_iNumAnimations; ++iAnimIndex)
	{
		ANIMATIONDATA AnimationData;
		const aiAnimation* pAnimation = m_pAIScene->mAnimations[iAnimIndex];

		strcpy_s(AnimationData.szName, pAnimation->mName.data);
		AnimationData.Duration = pAnimation->mDuration;
		AnimationData.TickPerSec = pAnimation->mTicksPerSecond;
		AnimationData.iNumChannels = pAnimation->mNumChannels;
		AnimationData.bIsLoop = true;

		if (0 >= AnimationData.iNumChannels)
			return E_FAIL;

		vector<CHANNELDATA> vecChannels;

		for (_uint iChannelIndex = 0; iChannelIndex < pAnimation->mNumChannels; ++iChannelIndex)
		{
			CHANNELDATA ChannelData;
			const aiNodeAnim* pChannel = pAnimation->mChannels[iChannelIndex];

			strcpy_s(ChannelData.szName, pChannel->mNodeName.data);
			ChannelData.iNumKeyFrames = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
			ChannelData.iNumKeyFrames = max(ChannelData.iNumKeyFrames, pChannel->mNumPositionKeys);

			if (0 >= ChannelData.iNumKeyFrames)
				return E_FAIL;

			vector<KEYFRAME> vecKeyFrames;

			_float3 vScale;
			_float4 vRotation;
			_float3 vTranslation;

			for (_uint iFrameIndex = 0; iFrameIndex < ChannelData.iNumKeyFrames; ++iFrameIndex)
			{
				KEYFRAME KeyFrame;

				if (pChannel->mNumScalingKeys > iFrameIndex)
				{
					std::memcpy(&vScale, &pChannel->mScalingKeys[iFrameIndex].mValue, sizeof(_float3));
					KeyFrame.Time = pChannel->mScalingKeys[iFrameIndex].mTime;
				}

				if (pChannel->mNumRotationKeys > iFrameIndex)
				{
					/* Rotation은 쿼터니언으로 되어있어서 직접값 넣어줘야됨.*/
					/* 왜냐하면 w x y z 로 저장되어있음 ㅋㅋ*/
					vRotation.x = pChannel->mRotationKeys[iFrameIndex].mValue.x;
					vRotation.y = pChannel->mRotationKeys[iFrameIndex].mValue.y;
					vRotation.z = pChannel->mRotationKeys[iFrameIndex].mValue.z;
					vRotation.w = pChannel->mRotationKeys[iFrameIndex].mValue.w;
					KeyFrame.Time = pChannel->mRotationKeys[iFrameIndex].mTime;
				}

				if (pChannel->mNumPositionKeys > iFrameIndex)
				{
					std::memcpy(&vTranslation, &pChannel->mPositionKeys[iFrameIndex].mValue, sizeof(_float3));
					KeyFrame.Time = pChannel->mPositionKeys[iFrameIndex].mTime;
				}

				KeyFrame.vScale = vScale;
				KeyFrame.vRotation = vRotation;
				KeyFrame.vTranslation = vTranslation;

				vecKeyFrames.push_back(KeyFrame);
			}
			ChannelData.vecKeyFrames = vecKeyFrames;
			vecChannels.push_back(ChannelData);
		}
		AnimationData.vecChannels = vecChannels;
		vecAnimation.push_back(AnimationData);
	}
	Data.vecAnimations = vecAnimation;

	return S_OK;
}

HRESULT CModelLoader::Ready_Materials(const string& strFilePath, MODEL_BINARYDATA& Data)
{
	Data.iNumMaterials = m_pAIScene->mNumMaterials;
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

void CModelLoader::ResetData()
{
	m_FilePathList.clear();

	for (auto& Model : m_vecDatas)
		Safe_Delete_BinaryData(Model);
	m_vecDatas.clear();
	m_vecBones.clear();
}

void CModelLoader::Free()
{
	ResetData();
	Safe_Release(m_pGameInstance);
}
