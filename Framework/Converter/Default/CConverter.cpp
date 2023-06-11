#include "CConverter.h"

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
	WriteNonAnimModels("NonAnimModelsFile.dat");
	m_FilePathList.clear();
	
	m_vecDatas.clear();

	ReadNonAnimModels("NonAnimModelsFile.dat");
	/*m_pGameInstance->Extraction_Data("../Bin/Resources/AnimModels", ".fbx", m_FilePathList);
	ConvertBinary();
	WriteAnimModels("Bin/AnimModelsFile.txt");*/

	PostQuitMessage(0);
}

void CConverter::WriteNonAnimModels(const string& strFileName)
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

		/* Write szFilePath*/
		iPathLength = lstrlen(m_vecDatas[iDataIndex].szFilePath) + 1;
		WriteFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, m_vecDatas[iDataIndex].szFilePath, sizeof(_tchar) * iPathLength, &dwByte, nullptr);

		/* Write iNumMeshes */
		WriteFile(hFile, &m_vecDatas[iDataIndex].iNumMeshes, sizeof(_uint), &dwByte, nullptr);

		/* Write iNumMaterials */
		WriteFile(hFile, &m_vecDatas[iDataIndex].iNumMaterials, sizeof(_uint), &dwByte, nullptr);

		for (_uint iMeshIndex = 0; iMeshIndex < m_vecDatas[iDataIndex].iNumMeshes; ++iMeshIndex)
		{
			/* Write szMesh Name*/
			_uint iNameLength = strlen(m_vecDatas[iDataIndex].pMeshData[iMeshIndex].szName) + 1;
			WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshData[iMeshIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Write szMaterialTexturePath */
			_uint iTexturePathlength = { 0 };
			for (_uint iMtrlTexIndex = 0; iMtrlTexIndex < 21; ++iMtrlTexIndex)
			{
				iTexturePathlength = lstrlen(m_vecDatas[iDataIndex].pMeshData[iMeshIndex].szMaterialTexturePath[iMtrlTexIndex]);
				WriteFile(hFile, &iTexturePathlength, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, m_vecDatas[iDataIndex].pMeshData[iMeshIndex].szMaterialTexturePath[iMtrlTexIndex], sizeof(_tchar) * iTexturePathlength, &dwByte, nullptr);
			}

			/* Write iMaterialIndex */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumVertices */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumVertices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumIndices */
			WriteFile(hFile, &m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumIndices, sizeof(_uint), &dwByte, nullptr);

			/* Write pVertices */
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshData[iMeshIndex].pVertices, sizeof(VTXMESH) * 
				m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumVertices, &dwByte, nullptr);
			
			/* Write pIndices */
			WriteFile(hFile, m_vecDatas[iDataIndex].pMeshData[iMeshIndex].pIndices, sizeof(_ulong) *
				m_vecDatas[iDataIndex].pMeshData[iMeshIndex].iNumIndices, &dwByte, nullptr);
		}
	}

	CloseHandle(hFile);
}

void CConverter::WriteAnimModels(const string& strFileName)
{
}

void CConverter::ReadNonAnimModels(const string& strFileName)
{
	m_FilePathList.clear();

	_char szFullPath[MAX_PATH] = { "" };
	GetCurrentDirectoryA(MAX_PATH, szFullPath);
	string strFullPath = szFullPath;

	_uint iPosLength = strFullPath.rfind("Default");

	strFullPath += "/" + strFileName;

	HANDLE hFile = CreateFileA(strFullPath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	_ulong dwByte = { 0 };

	while (true)
	{
		NONANIM_MODEL_BINARYDATA Data;
		ZeroMemory(&Data, sizeof Data);

		_uint iPathLength = { 0 };
		_char szFilePath[MAX_PATH] = { "" };
		/* Write strFilePath */
		ReadFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, szFilePath, sizeof(_char) * iPathLength, &dwByte, nullptr);

		if (0 != dwByte)
			m_FilePathList.push_back(szFilePath);

		/* Read szFilePath*/
		ReadFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, Data.szFilePath, sizeof(_tchar) * iPathLength, &dwByte, nullptr);

		/* Read iNumMeshes */
		ReadFile(hFile, &Data.iNumMeshes, sizeof(_uint), &dwByte, nullptr);

		/* Read iNumMaterials */
		ReadFile(hFile, &Data.iNumMaterials, sizeof(_uint), &dwByte, nullptr);

		MESHDATA* pMeshData = new MESHDATA[Data.iNumMeshes];
		for (_uint iMeshIndex = 0; iMeshIndex < Data.iNumMeshes; ++iMeshIndex)
		{
			/* Read szMesh Name*/
			_uint iNameLength = { 0 };
			ReadFile(hFile, &iNameLength, sizeof(_uint), & dwByte, nullptr);
			ReadFile(hFile, pMeshData[iMeshIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Read szMaterialTexturePath */
			_uint iTexturePathlength = { 0 };
			for (_uint iMtrlTexIndex = 0; iMtrlTexIndex < 21; ++iMtrlTexIndex)
			{
				iTexturePathlength = lstrlen(pMeshData[iMeshIndex].szMaterialTexturePath[iMtrlTexIndex]);
				ReadFile(hFile, &iTexturePathlength, sizeof(_uint), &dwByte, nullptr);
				ReadFile(hFile, pMeshData[iMeshIndex].szMaterialTexturePath[iMtrlTexIndex], sizeof(_tchar) * iTexturePathlength, &dwByte, nullptr);
			}

			/* Read iMaterialIndex */
			ReadFile(hFile, &pMeshData[iMeshIndex].iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

			/* Read iNumVertices */
			ReadFile(hFile, &pMeshData[iMeshIndex].iNumVertices, sizeof(_uint), &dwByte, nullptr);

			/* Read iNumIndices */
			ReadFile(hFile, &pMeshData[iMeshIndex].iNumIndices, sizeof(_uint), &dwByte, nullptr);

			VTXMESH* pVertices = new VTXMESH[pMeshData[iMeshIndex].iNumVertices];
			ZeroMemory(pVertices, sizeof(VTXMESH) * pMeshData[iMeshIndex].iNumVertices);

			/* Read pVertices */
			ReadFile(hFile, pVertices, sizeof(VTXMESH) * pMeshData[iMeshIndex].iNumVertices, &dwByte, nullptr);

			_ulong* pIndices = new _ulong[pMeshData[iMeshIndex].iNumIndices];
			ZeroMemory(pIndices, sizeof(_ulong) * pMeshData[iMeshIndex].iNumIndices);

			/* Read pIndices */
			ReadFile(hFile, pIndices, sizeof(_ulong) * pMeshData[iMeshIndex].iNumIndices, &dwByte, nullptr);

			pMeshData[iMeshIndex].pVertices = pVertices;
			pMeshData[iMeshIndex].pIndices = pIndices;
		}

		if (0 == dwByte)
		{
			Safe_Delete_Array(pMeshData);
			break;
		}

		Data.pMeshData = pMeshData;

		m_vecDatas.push_back(Data);
	}
}
	
void CConverter::ReadAnimModels(const string& strFileName)
{
}

void CConverter::ConvertBinary_NonAnimModel()
{
	m_vecDatas.reserve(m_FilePathList.size());

	_uint iIndex = 0;
	for (auto& Path : m_FilePathList)
	{
		NONANIM_MODEL_BINARYDATA BinDataDesc;
		ZeroMemory(&BinDataDesc, sizeof BinDataDesc);

		m_pAIScene = m_Importer.ReadFile(Path, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
		
		if (nullptr == m_pAIScene)
			return;

		lstrcpy(BinDataDesc.szFilePath, CGameInstance::GetInstance()->strToWStr(Path).c_str());
		
		m_iNumMeshes = m_pAIScene->mNumMeshes;
		BinDataDesc.iNumMeshes = m_iNumMeshes;

		_matrix PivotMatrix = XMMatrixIdentity();
		PivotMatrix = XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(-90.f));

		BinDataDesc.pMeshData = new MESHDATA[m_iNumMeshes];
		ZeroMemory(BinDataDesc.pMeshData, sizeof(MESHDATA) * m_iNumMeshes);

		if (FAILED(Ready_Meshes_NonAnimModel(PivotMatrix, BinDataDesc.pMeshData)))
		{
			MSG_BOX("Failed_Ready_Meshes");
			return;
		}

		BinDataDesc.iNumMaterials = m_pAIScene->mNumMaterials;
		if (FAILED(Ready_Materials_NonAnimModel(Path, BinDataDesc.pMeshData)))
		{
			MSG_BOX("Failed_Ready_Materiala");
			return;
		}
		m_vecDatas.push_back(BinDataDesc);
		++iIndex;
	}
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
		pData[i].iNumVertices = m_iNumVertices;

		m_iNumIndices = { pAIMesh->mNumFaces * 3 };
		pData[i].iNumIndices = m_iNumIndices;

		pData[i].pVertices = new VTXMESH[m_iNumVertices];

		for (_uint j = 0; j < m_iNumVertices; ++j)
		{
			memcpy(&pData[i].pVertices[j].vPosition, &pAIMesh->mVertices[j], sizeof(_float3));
			XMStoreFloat3(&pData[i].pVertices[j].vPosition,
				XMVector3TransformCoord(XMLoadFloat3(&pData[i].pVertices[j].vPosition), PivotMatrix));

			memcpy(&pData[i].pVertices[j].vNormal, &pAIMesh->mNormals[j], sizeof(_float3));
			XMStoreFloat3(&pData[i].pVertices[j].vNormal,
				XMVector3TransformCoord(XMLoadFloat3(&pData[i].pVertices[j].vNormal), PivotMatrix));

			memcpy(&pData[i].pVertices[j].vTexCoord, &pAIMesh->mTextureCoords[0][j], sizeof(_float2));
			memcpy(&pData[i].pVertices[j].vTangent, &pAIMesh->mTangents[j], sizeof(_float3));
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

HRESULT CConverter::Ready_Materials_NonAnimModel(const string& strFilePath, OUT MESHDATA* pData)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

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

			lstrcpy(pData->szMaterialTexturePath[j], m_pGameInstance->strToWStr(strFullPath).c_str());
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

void CConverter::Free()
{
	Safe_Release(m_pGameInstance);
}
