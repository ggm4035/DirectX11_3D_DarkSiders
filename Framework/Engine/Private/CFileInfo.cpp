#include "CFileInfo.h"
#include <locale>
#include <codecvt>

IMPLEMENT_SINGLETON(CFileInfo)

CFileInfo::CFileInfo()
{
}

wstring CFileInfo::ConvertRelatePath(const wstring& strFullPath)
{
	_tchar szRelativePath[MAX_PATH] = L"";
	_tchar szCurDirPath[MAX_PATH] = L"";

	// 현재 경로를 가져오는 함수
	GetCurrentDirectory(MAX_PATH, szCurDirPath);

	// 현재 경로(szCurDirPath) 에서 인자로 받은 경로(strFullPath) 까지 가는 상대경로 생성 하는 함수
	// 단, 같은 드라이브 내에서만 가능(C:, D:...)
	PathRelativePathTo(szRelativePath,
		szCurDirPath,
		FILE_ATTRIBUTE_DIRECTORY,
		strFullPath.c_str(),
		FILE_ATTRIBUTE_DIRECTORY);

	return wstring(szRelativePath);
}

//L"파일 경로/경로/*.fbx" 면 해당 파일 내의 모든 .fbx 파일을 찾는거임
//DWORD dwFileAttributes; // 파일의 속성 숨김, 읽기전용, 시스템파일인지
//WCHAR  cFileName[MAX_PATH];// 파일의 이름
//FILE_ATTRIBUTE_ARCHIVE;
HRESULT CFileInfo::Extraction_Data(const string& strPath, const _char* pExt, OUT list<string>& FilePathList)
{
	HANDLE hFind;

	WIN32_FIND_DATAA FindFileData;

	string strFindTag = strPath + "/*.*";

	hFind = FindFirstFileA(strFindTag.c_str(), &FindFileData);

	while (FindNextFileA(hFind, &FindFileData))
	{
		if (0 == strcmp(FindFileData.cFileName, ".."))
			continue;

		else if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			Extraction_Data(strPath + "/" + FindFileData.cFileName, pExt, FilePathList);

		else
		{
			if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_SYSTEM)
				continue;

			string strFilePath = strPath + "/" + FindFileData.cFileName;

			_char szExt[MAX_PATH] = { "" };

			_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

			if (0 != strcmp(szExt, pExt))
				continue;

			FilePathList.push_back(strFilePath);
		}
	}

	return S_OK;
}

void CFileInfo::WriteModels(const string& strFilePath, const list<string>& FilePathList, const vector<MODEL_BINARYDATA>& vecData)
{
	auto strPath = FilePathList.begin();

	HANDLE hFile = 0;

	for (_uint iDataIndex = 0; iDataIndex < FilePathList.size(); ++iDataIndex)
	{
		string strFullPath = strFilePath;
		_char szFileName[MAX_PATH] = { "" };
		_splitpath_s(strPath->c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		strFullPath = strFullPath + szFileName + ".dat";

		hFile = CreateFileA(strFullPath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		_ulong dwByte = { 0 };

		/* Write strFilePath */
		_uint iPathLength = strlen(strPath->c_str()) + 1;
		WriteFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, strPath->c_str(), sizeof(_char) * iPathLength, &dwByte, nullptr);
		++strPath;

		/*===== BINARY DATAS ======*/
		/* Write szTag */
		iPathLength = lstrlen(vecData[iDataIndex].szTag) + 1;
		WriteFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, vecData[iDataIndex].szTag, sizeof(_tchar) * iPathLength, &dwByte, nullptr);

		/* Write iNumMeshes */
		WriteFile(hFile, &vecData[iDataIndex].iNumMeshes, sizeof(_uint), &dwByte, nullptr);

		/* Write iNumMaterials */
		WriteFile(hFile, &vecData[iDataIndex].iNumMaterials, sizeof(_uint), &dwByte, nullptr);

		/* Write szMaterialTexturePath */
		_uint iTexturePathlength = { 0 };
		for (_uint iMaterialIndex = 0; iMaterialIndex < vecData[iDataIndex].iNumMaterials; ++iMaterialIndex)
		{
			for (_uint iTextureIndex = 0; iTextureIndex < 21; ++iTextureIndex)
			{
				iTexturePathlength = lstrlen(vecData[iDataIndex].pMaterialPaths[iMaterialIndex].szMaterialTexturePath[iTextureIndex]) + 1;
				WriteFile(hFile, &iTexturePathlength, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, vecData[iDataIndex].pMaterialPaths[iMaterialIndex].szMaterialTexturePath[iTextureIndex],
					sizeof(_tchar) * iTexturePathlength, &dwByte, nullptr);
			}
		}

		/* Write iNumBones */
		WriteFile(hFile, &vecData[iDataIndex].iNumBones, sizeof(_uint), &dwByte, nullptr);

		/*===== BONE DATAS ======*/
		if (0 < vecData[iDataIndex].iNumBones)
		{
			for (_uint iBoneIndex = 0; iBoneIndex < vecData[iDataIndex].iNumBones; ++iBoneIndex)
			{
				/* Write szName_Bone */
				_uint iNameLength = strlen(vecData[iDataIndex].pBoneDatas[iBoneIndex].szName) + 1;
				WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, vecData[iDataIndex].pBoneDatas[iBoneIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

				/* Write iParentIdx */
				WriteFile(hFile, &vecData[iDataIndex].pBoneDatas[iBoneIndex].iParentIdx, sizeof(_uint), &dwByte, nullptr);

				/* Write iIndex */
				WriteFile(hFile, &vecData[iDataIndex].pBoneDatas[iBoneIndex].iIndex, sizeof(_uint), &dwByte, nullptr);

				/* Write TransformationMatrix */
				WriteFile(hFile, &vecData[iDataIndex].pBoneDatas[iBoneIndex].TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

				/* Write OffsetMatrix */
				WriteFile(hFile, &vecData[iDataIndex].pBoneDatas[iBoneIndex].OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);
			}
		}

		/* Write iNumAnimations */
		WriteFile(hFile, &vecData[iDataIndex].iNumAnimations, sizeof(_uint), &dwByte, nullptr);

		/*===== ANIMATION DATAS ======*/
		for (_uint iAnimIndex = 0; iAnimIndex < vecData[iDataIndex].iNumAnimations; ++iAnimIndex)
		{
			/* Write szName */
			_uint iNameLength = strlen(vecData[iDataIndex].pAnimations[iAnimIndex].szName) + 1;
			WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, vecData[iDataIndex].pAnimations[iAnimIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Write Duration */
			WriteFile(hFile, &vecData[iDataIndex].pAnimations[iAnimIndex].Duration, sizeof(_double), &dwByte, nullptr);

			/* Write TickPerSec */
			WriteFile(hFile, &vecData[iDataIndex].pAnimations[iAnimIndex].TickPerSec, sizeof(_double), &dwByte, nullptr);

			/* Write iNumChannels */
			WriteFile(hFile, &vecData[iDataIndex].pAnimations[iAnimIndex].iNumChannels, sizeof(_uint), &dwByte, nullptr);

			/* Write bIsLoop */
			WriteFile(hFile, &vecData[iDataIndex].pAnimations[iAnimIndex].bIsLoop, sizeof(_bool), &dwByte, nullptr);

			/* Write bIsFollowAnimation */
			WriteFile(hFile, &vecData[iDataIndex].pAnimations[iAnimIndex].bIsFollowAnimation, sizeof(_bool), &dwByte, nullptr);

			/* Write iNumRanges */
			WriteFile(hFile, &vecData[iDataIndex].pAnimations[iAnimIndex].iNumRanges, sizeof(_uint), &dwByte, nullptr);

			/*===== TIMERANGE DATAS ======*/
			for (_uint iRangeIndex = 0; iRangeIndex < vecData[iDataIndex].pAnimations[iAnimIndex].iNumRanges; ++iRangeIndex)
			{
				/* Write pTypes */
				WriteFile(hFile, vecData[iDataIndex].pAnimations[iAnimIndex].pTimeRanges[iRangeIndex].arrTypes, 
					sizeof(OBSERVERTYPE) * OBSERVERTYPE::TYPE_END, &dwByte, nullptr);

				/* Write fStartPoint */
				WriteFile(hFile, &vecData[iDataIndex].pAnimations[iAnimIndex].pTimeRanges[iRangeIndex].fStartPoint, sizeof(_float), &dwByte, nullptr);

				/* Write fEndPoint */
				WriteFile(hFile, &vecData[iDataIndex].pAnimations[iAnimIndex].pTimeRanges[iRangeIndex].fEndPoint, sizeof(_float), &dwByte, nullptr);
			}

			/*===== CHANNEL DATAS ======*/
			for (_uint iChannelIndex = 0; iChannelIndex < vecData[iDataIndex].pAnimations[iAnimIndex].iNumChannels; ++iChannelIndex)
			{
				/* Write szName */
				iNameLength = strlen(vecData[iDataIndex].pAnimations[iAnimIndex].pChannels[iChannelIndex].szName) + 1;
				WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, vecData[iDataIndex].pAnimations[iAnimIndex].pChannels[iChannelIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

				/* Write iNumKeyFrames */
				WriteFile(hFile, &vecData[iDataIndex].pAnimations[iAnimIndex].pChannels[iChannelIndex].iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);

				/* Write KeyFrame */
				WriteFile(hFile, vecData[iDataIndex].pAnimations[iAnimIndex].pChannels[iChannelIndex].pKeyFrames,
					sizeof(KEYFRAME) * vecData[iDataIndex].pAnimations[iAnimIndex].pChannels[iChannelIndex].iNumKeyFrames, &dwByte, nullptr);
			}
		}

		/*===== MESH DATAS ======*/
		for (_uint iMeshIndex = 0; iMeshIndex < vecData[iDataIndex].iNumMeshes; ++iMeshIndex)
		{
			/* Write szName_Mesh */
			_uint iNameLength = strlen(vecData[iDataIndex].pMeshDatas[iMeshIndex].szName) + 1;
			WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, vecData[iDataIndex].pMeshDatas[iMeshIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Write iMaterialIndex */
			WriteFile(hFile, &vecData[iDataIndex].pMeshDatas[iMeshIndex].iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumNonAnimVertices */
			WriteFile(hFile, &vecData[iDataIndex].pMeshDatas[iMeshIndex].iNumNonAnimVertices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumAnimVertices */
			WriteFile(hFile, &vecData[iDataIndex].pMeshDatas[iMeshIndex].iNumAnimVertices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumIndices */
			WriteFile(hFile, &vecData[iDataIndex].pMeshDatas[iMeshIndex].iNumIndices, sizeof(_uint), &dwByte, nullptr);

			/* Write iNumMeshBones */
			WriteFile(hFile, &vecData[iDataIndex].pMeshDatas[iMeshIndex].iNumMeshBones, sizeof(_uint), &dwByte, nullptr);

			/* Write pBoneIndices */
			WriteFile(hFile, vecData[iDataIndex].pMeshDatas[iMeshIndex].pBoneIndices, sizeof(_uint) *
				vecData[iDataIndex].pMeshDatas[iMeshIndex].iNumMeshBones, &dwByte, nullptr);

			/* Write pNonAnimVertices */
			if (0 < vecData[iDataIndex].pMeshDatas[iMeshIndex].iNumNonAnimVertices)
			{
				WriteFile(hFile, vecData[iDataIndex].pMeshDatas[iMeshIndex].pNonAnimVertices, sizeof(VTXMESH) *
					vecData[iDataIndex].pMeshDatas[iMeshIndex].iNumNonAnimVertices, &dwByte, nullptr);
			}

			/* Write pAnimVertices */
			if (0 < vecData[iDataIndex].pMeshDatas[iMeshIndex].iNumAnimVertices)
			{
				WriteFile(hFile, vecData[iDataIndex].pMeshDatas[iMeshIndex].pAnimVertices, sizeof(VTXANIMMESH) *
					vecData[iDataIndex].pMeshDatas[iMeshIndex].iNumAnimVertices, &dwByte, nullptr);
			}

			/* Write pIndices */
			WriteFile(hFile, vecData[iDataIndex].pMeshDatas[iMeshIndex].pIndices, sizeof(_ulong) *
				vecData[iDataIndex].pMeshDatas[iMeshIndex].iNumIndices, &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
}

void CFileInfo::ReadModels(const string& strFilePath, OUT list<string>& FilePathList, OUT vector<MODEL_BINARYDATA>& vecData)
{
	FilePathList.clear();

	HANDLE hFile = CreateFileA(strFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	_ulong dwByte = { 0 };

	while (true)
	{
		MODEL_BINARYDATA Data;
		ZeroMemory(&Data, sizeof Data);

		_uint iPathLength = { 0 };
		_char szFilePath[MAX_PATH] = { "" };
		/* Read strFilePath */
		ReadFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, szFilePath, sizeof(_char) * iPathLength, &dwByte, nullptr);

		if (0 != dwByte)
			FilePathList.push_back(szFilePath);

		/*========== BINARY DATAS ===========*/
		/* Read szTag */
		ReadFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, Data.szTag, sizeof(_tchar) * iPathLength, &dwByte, nullptr);

		/* Read iNumMeshes */
		ReadFile(hFile, &Data.iNumMeshes, sizeof(_uint), &dwByte, nullptr);

		/* Read iNumMaterials */
		ReadFile(hFile, &Data.iNumMaterials, sizeof(_uint), &dwByte, nullptr);

		/* Read pMaterialPaths */
		MATERIALPATH* pMaterialPath = { nullptr };
		if (0 < Data.iNumMaterials)
		{
			pMaterialPath = new MATERIALPATH[Data.iNumMaterials];
			ZeroMemory(pMaterialPath, sizeof(MATERIALPATH) * Data.iNumMaterials);
		}
		_uint iTexturePathlength = { 0 };
		for (_uint iMaterialIndex = 0; iMaterialIndex < Data.iNumMaterials; ++iMaterialIndex)
		{
			for (_uint iTextureIndex = 0; iTextureIndex < 21; ++iTextureIndex)
			{
				ReadFile(hFile, &iTexturePathlength, sizeof(_uint), &dwByte, nullptr);
				ReadFile(hFile, pMaterialPath[iMaterialIndex].szMaterialTexturePath[iTextureIndex],
					sizeof(_tchar) * iTexturePathlength, &dwByte, nullptr);
			}
		}

		/* Read iNumBones */
		ReadFile(hFile, &Data.iNumBones, sizeof(_uint), &dwByte, nullptr);

		/*========== BONE DATAS ===========*/
		BONEDATA* pBoneData = { nullptr };
		if (0 < Data.iNumBones)
		{
			pBoneData = new BONEDATA[Data.iNumBones];
			ZeroMemory(pBoneData, sizeof(BONEDATA) * Data.iNumBones);
		}

		for (_uint iBoneIndex = 0; iBoneIndex < Data.iNumBones; ++iBoneIndex)
		{
			/* Read szName_Bone */
			_uint iNameLength = { 0 };
			ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, pBoneData[iBoneIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Read iParentIdx */
			ReadFile(hFile, &pBoneData[iBoneIndex].iParentIdx, sizeof(_uint), &dwByte, nullptr);

			/* Read iIndex */
			ReadFile(hFile, &pBoneData[iBoneIndex].iIndex, sizeof(_uint), &dwByte, nullptr);

			/* Read TransformationMatrix */
			ReadFile(hFile, &pBoneData[iBoneIndex].TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

			/* Read OffsetMatrix */
			ReadFile(hFile, &pBoneData[iBoneIndex].OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);
		}

		/* Read iNumAnimations */
		ReadFile(hFile, &Data.iNumAnimations, sizeof(_uint), &dwByte, nullptr);

		/*========== ANIMATION DATAS ===========*/
		ANIMATIONDATA* pAnimation = { nullptr };
		if (0 < Data.iNumAnimations)
		{
			pAnimation = new ANIMATIONDATA[Data.iNumAnimations];
			ZeroMemory(pAnimation, sizeof(ANIMATIONDATA) * Data.iNumAnimations);
		}

		for (_uint iAnimIndex = 0; iAnimIndex < Data.iNumAnimations; ++iAnimIndex)
		{
			/* Read szName */
			_uint iNameLength = { 0 };
			ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, pAnimation[iAnimIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Read Duration */
			ReadFile(hFile, &pAnimation[iAnimIndex].Duration, sizeof(_double), &dwByte, nullptr);

			/* Read TickPerSec */
			ReadFile(hFile, &pAnimation[iAnimIndex].TickPerSec, sizeof(_double), &dwByte, nullptr);

			/* Read iNumChannels */
			ReadFile(hFile, &pAnimation[iAnimIndex].iNumChannels, sizeof(_uint), &dwByte, nullptr);

			/* Read bIsLoop */
			ReadFile(hFile, &pAnimation[iAnimIndex].bIsLoop, sizeof(_bool), &dwByte, nullptr);

			/* Read bIsFollowAnimation */
			ReadFile(hFile, &pAnimation[iAnimIndex].bIsFollowAnimation, sizeof(_bool), &dwByte, nullptr);

			/* Read iNumRanges */
			ReadFile(hFile, &pAnimation[iAnimIndex].iNumRanges, sizeof(_uint), &dwByte, nullptr);

			/*===== TIMERANGE DATAS ======*/
			TIMERANGE* pTimeRange = { nullptr };
			if (0 < pAnimation[iAnimIndex].iNumRanges)
			{
				pTimeRange = new TIMERANGE[pAnimation[iAnimIndex].iNumRanges];
				ZeroMemory(pTimeRange, sizeof(TIMERANGE) * pAnimation[iAnimIndex].iNumRanges);
			}
			for (_uint iRangeIndex = 0; iRangeIndex < pAnimation[iAnimIndex].iNumRanges; ++iRangeIndex)
			{
				for (auto& Type : pTimeRange[iRangeIndex].arrTypes)
					Type = OBSERVERTYPE::TYPE_END;

				/* Read arrTypes */
				ReadFile(hFile, pTimeRange[iRangeIndex].arrTypes, sizeof(OBSERVERTYPE) * OBSERVERTYPE::TYPE_END, &dwByte, nullptr);

				/* Read fStartPoint */
				ReadFile(hFile, &pTimeRange[iRangeIndex].fStartPoint, sizeof(_float), &dwByte, nullptr);

				/* Read fEndPoint */
				ReadFile(hFile, &pTimeRange[iRangeIndex].fEndPoint, sizeof(_float), &dwByte, nullptr);
			}

			/*========== CHANNEL DATAS ===========*/
			CHANNELDATA* pChannel = { nullptr };
			if (0 < pAnimation[iAnimIndex].iNumChannels)
			{
				pChannel = new CHANNELDATA[pAnimation[iAnimIndex].iNumChannels];
				ZeroMemory(pChannel, sizeof(CHANNELDATA) * pAnimation[iAnimIndex].iNumChannels);
			}

			for (_uint iChannelIndex = 0; iChannelIndex < pAnimation[iAnimIndex].iNumChannels; ++iChannelIndex)
			{
				/* Read szName */
				iNameLength = strlen(pChannel[iChannelIndex].szName) + 1;
				ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
				ReadFile(hFile, pChannel[iChannelIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

				/* Read iNumKeyFrames */
				ReadFile(hFile, &pChannel[iChannelIndex].iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);

				/* Read KeyFrame */
				KEYFRAME* pKeyFrame = { nullptr };
				if (0 < pChannel[iChannelIndex].iNumKeyFrames)
				{
					pKeyFrame = new KEYFRAME[pChannel[iChannelIndex].iNumKeyFrames];
					ZeroMemory(pKeyFrame, sizeof(KEYFRAME) * pChannel[iChannelIndex].iNumKeyFrames);
				}

				ReadFile(hFile, pKeyFrame, sizeof(KEYFRAME) * pChannel[iChannelIndex].iNumKeyFrames, &dwByte, nullptr);
				pChannel[iChannelIndex].pKeyFrames = pKeyFrame;
			}
			pAnimation[iAnimIndex].pTimeRanges = pTimeRange;
			pAnimation[iAnimIndex].pChannels = pChannel;
		}

		/*========== MESH DATAS ===========*/
		MESHDATA* pMeshData = new MESHDATA[Data.iNumMeshes];
		ZeroMemory(pMeshData, sizeof(MESHDATA) * Data.iNumMeshes);
		for (_uint iMeshIndex = 0; iMeshIndex < Data.iNumMeshes; ++iMeshIndex)
		{
			/* Read szMesh Name*/
			_uint iNameLength = { 0 };
			ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, pMeshData[iMeshIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Read iMaterialIndex */
			ReadFile(hFile, &pMeshData[iMeshIndex].iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

			/* Read iNumNonAnimVertices */
			ReadFile(hFile, &pMeshData[iMeshIndex].iNumNonAnimVertices, sizeof(_uint), &dwByte, nullptr);

			/* Read iNumAnimVertices */
			ReadFile(hFile, &pMeshData[iMeshIndex].iNumAnimVertices, sizeof(_uint), &dwByte, nullptr);

			/* Read iNumIndices */
			ReadFile(hFile, &pMeshData[iMeshIndex].iNumIndices, sizeof(_uint), &dwByte, nullptr);

			/* Read iNumMeshBones */
			ReadFile(hFile, &pMeshData[iMeshIndex].iNumMeshBones, sizeof(_uint), &dwByte, nullptr);

			/* Read pBoneIndices */
			pMeshData[iMeshIndex].pBoneIndices = new _uint[pMeshData[iMeshIndex].iNumMeshBones];
			ZeroMemory(pMeshData[iMeshIndex].pBoneIndices, sizeof(_uint) * pMeshData[iMeshIndex].iNumMeshBones);
			ReadFile(hFile, pMeshData[iMeshIndex].pBoneIndices, sizeof(_uint) * pMeshData[iMeshIndex].iNumMeshBones, &dwByte, nullptr);

			/* Read pNonAnimVertices */
			VTXMESH* pNonAnimVertices = { nullptr };
			if (0 < pMeshData[iMeshIndex].iNumNonAnimVertices)
			{
				pNonAnimVertices = new VTXMESH[pMeshData[iMeshIndex].iNumNonAnimVertices];
				ZeroMemory(pNonAnimVertices, sizeof(VTXMESH) * pMeshData[iMeshIndex].iNumNonAnimVertices);
				ReadFile(hFile, pNonAnimVertices, sizeof(VTXMESH) * pMeshData[iMeshIndex].iNumNonAnimVertices, &dwByte, nullptr);
			}

			/* Read pAnimVertices */
			VTXANIMMESH* pAnimVertices = { nullptr };
			if (0 < pMeshData[iMeshIndex].iNumAnimVertices)
			{
				pAnimVertices = new VTXANIMMESH[pMeshData[iMeshIndex].iNumAnimVertices];
				ZeroMemory(pAnimVertices, sizeof(VTXANIMMESH) * pMeshData[iMeshIndex].iNumAnimVertices);
				ReadFile(hFile, pAnimVertices, sizeof(VTXANIMMESH) * pMeshData[iMeshIndex].iNumAnimVertices, &dwByte, nullptr);
			}

			/* Read pIndices */
			_ulong* pIndices = new _ulong[pMeshData[iMeshIndex].iNumIndices];
			ZeroMemory(pIndices, sizeof(_ulong) * pMeshData[iMeshIndex].iNumIndices);
			ReadFile(hFile, pIndices, sizeof(_ulong) * pMeshData[iMeshIndex].iNumIndices, &dwByte, nullptr);

			pMeshData[iMeshIndex].pNonAnimVertices = pNonAnimVertices;
			pMeshData[iMeshIndex].pAnimVertices = pAnimVertices;
			pMeshData[iMeshIndex].pIndices = pIndices;
		}

		if (0 == dwByte)
		{
			Safe_Delete_Array(pMeshData);
			break;
		}

		Data.pAnimations = pAnimation;
		Data.pMaterialPaths = pMaterialPath;
		Data.pBoneDatas = pBoneData;
		Data.pMeshDatas = pMeshData;

		vecData.push_back(Data);
	}

	CloseHandle(hFile);
}

void CFileInfo::ReadModel(const string& strFilePath, OUT string& FilePath, OUT MODEL_BINARYDATA& Data)
{
	HANDLE hFile = CreateFileA(strFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	_ulong dwByte = { 0 };

	ZeroMemory(&Data, sizeof Data);

	_uint iPathLength = { 0 };
	_char szFilePath[MAX_PATH] = { "" };
	/* Read strFilePath */
	ReadFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, szFilePath, sizeof(_char) * iPathLength, &dwByte, nullptr);

	if (0 != dwByte)
		FilePath = szFilePath;

	/*========== BINARY DATAS ===========*/
	/* Read szTag */
	ReadFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, Data.szTag, sizeof(_tchar) * iPathLength, &dwByte, nullptr);

	/* Read iNumMeshes */
	ReadFile(hFile, &Data.iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	/* Read iNumMaterials */
	ReadFile(hFile, &Data.iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	/* Read pMaterialPaths */
	MATERIALPATH* pMaterialPath = { nullptr };
	if (0 < Data.iNumMaterials)
	{
		pMaterialPath = new MATERIALPATH[Data.iNumMaterials];
		ZeroMemory(pMaterialPath, sizeof(MATERIALPATH) * Data.iNumMaterials);
	}
	_uint iTexturePathlength = { 0 };
	for (_uint iMaterialIndex = 0; iMaterialIndex < Data.iNumMaterials; ++iMaterialIndex)
	{
		for (_uint iTextureIndex = 0; iTextureIndex < 21; ++iTextureIndex)
		{
			ReadFile(hFile, &iTexturePathlength, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, pMaterialPath[iMaterialIndex].szMaterialTexturePath[iTextureIndex],
				sizeof(_tchar) * iTexturePathlength, &dwByte, nullptr);
		}
	}

	/* Read iNumBones */
	ReadFile(hFile, &Data.iNumBones, sizeof(_uint), &dwByte, nullptr);

	/*========== BONE DATAS ===========*/
	BONEDATA* pBoneData = { nullptr };
	if (0 < Data.iNumBones)
	{
		pBoneData = new BONEDATA[Data.iNumBones];
		ZeroMemory(pBoneData, sizeof(BONEDATA) * Data.iNumBones);
	}

	for (_uint iBoneIndex = 0; iBoneIndex < Data.iNumBones; ++iBoneIndex)
	{
		/* Read szName_Bone */
		_uint iNameLength = { 0 };
		ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, pBoneData[iBoneIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

		/* Read iParentIdx */
		ReadFile(hFile, &pBoneData[iBoneIndex].iParentIdx, sizeof(_uint), &dwByte, nullptr);

		/* Read iIndex */
		ReadFile(hFile, &pBoneData[iBoneIndex].iIndex, sizeof(_uint), &dwByte, nullptr);

		/* Read TransformationMatrix */
		ReadFile(hFile, &pBoneData[iBoneIndex].TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

		/* Read OffsetMatrix */
		ReadFile(hFile, &pBoneData[iBoneIndex].OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);
	}

	/* Read iNumAnimations */
	ReadFile(hFile, &Data.iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	/*========== ANIMATION DATAS ===========*/
	ANIMATIONDATA* pAnimation = { nullptr };
	if (0 < Data.iNumAnimations)
	{
		pAnimation = new ANIMATIONDATA[Data.iNumAnimations];
		ZeroMemory(pAnimation, sizeof(ANIMATIONDATA) * Data.iNumAnimations);
	}

	for (_uint iAnimIndex = 0; iAnimIndex < Data.iNumAnimations; ++iAnimIndex)
	{
		/* Read szName */
		_uint iNameLength = { 0 };
		ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, pAnimation[iAnimIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

		/* Read Duration */
		ReadFile(hFile, &pAnimation[iAnimIndex].Duration, sizeof(_double), &dwByte, nullptr);

		/* Read TickPerSec */
		ReadFile(hFile, &pAnimation[iAnimIndex].TickPerSec, sizeof(_double), &dwByte, nullptr);

		/* Read iNumChannels */
		ReadFile(hFile, &pAnimation[iAnimIndex].iNumChannels, sizeof(_uint), &dwByte, nullptr);

		/* Read bIsLoop */
		ReadFile(hFile, &pAnimation[iAnimIndex].bIsLoop, sizeof(_bool), &dwByte, nullptr);

		/* Read bIsFollowAnimation */
		ReadFile(hFile, &pAnimation[iAnimIndex].bIsFollowAnimation, sizeof(_bool), &dwByte, nullptr);

		/* Read iNumRanges */
		ReadFile(hFile, &pAnimation[iAnimIndex].iNumRanges, sizeof(_uint), &dwByte, nullptr);

		/*===== TIMERANGE DATAS ======*/
		TIMERANGE* pTimeRange = { nullptr };
		if (0 < pAnimation[iAnimIndex].iNumRanges)
		{
			pTimeRange = new TIMERANGE[pAnimation[iAnimIndex].iNumRanges];
			ZeroMemory(pTimeRange, sizeof(TIMERANGE) * pAnimation[iAnimIndex].iNumRanges);
		}
		for (_uint iRangeIndex = 0; iRangeIndex < pAnimation[iAnimIndex].iNumRanges; ++iRangeIndex)
		{
			for (auto& Type : pTimeRange[iRangeIndex].arrTypes)
				Type = OBSERVERTYPE::TYPE_END;

			/* Read arrTypes */
			ReadFile(hFile, pTimeRange[iRangeIndex].arrTypes, sizeof(OBSERVERTYPE) * OBSERVERTYPE::TYPE_END, &dwByte, nullptr);

			/* Read fStartPoint */
			ReadFile(hFile, &pTimeRange[iRangeIndex].fStartPoint, sizeof(_float), &dwByte, nullptr);

			/* Read fEndPoint */
			ReadFile(hFile, &pTimeRange[iRangeIndex].fEndPoint, sizeof(_float), &dwByte, nullptr);
		}

		/*========== CHANNEL DATAS ===========*/
		CHANNELDATA* pChannel = { nullptr };
		if (0 < pAnimation[iAnimIndex].iNumChannels)
		{
			pChannel = new CHANNELDATA[pAnimation[iAnimIndex].iNumChannels];
			ZeroMemory(pChannel, sizeof(CHANNELDATA) * pAnimation[iAnimIndex].iNumChannels);
		}

		for (_uint iChannelIndex = 0; iChannelIndex < pAnimation[iAnimIndex].iNumChannels; ++iChannelIndex)
		{
			/* Read szName */
			iNameLength = strlen(pChannel[iChannelIndex].szName) + 1;
			ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, pChannel[iChannelIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Read iNumKeyFrames */
			ReadFile(hFile, &pChannel[iChannelIndex].iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);

			/* Read KeyFrame */
			KEYFRAME* pKeyFrame = { nullptr };
			if (0 < pChannel[iChannelIndex].iNumKeyFrames)
			{
				pKeyFrame = new KEYFRAME[pChannel[iChannelIndex].iNumKeyFrames];
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME) * pChannel[iChannelIndex].iNumKeyFrames);
			}

			ReadFile(hFile, pKeyFrame, sizeof(KEYFRAME) * pChannel[iChannelIndex].iNumKeyFrames, &dwByte, nullptr);
			pChannel[iChannelIndex].pKeyFrames = pKeyFrame;
		}
		pAnimation[iAnimIndex].pTimeRanges = pTimeRange;
		pAnimation[iAnimIndex].pChannels = pChannel;
	}

	/*========== MESH DATAS ===========*/
	MESHDATA* pMeshData = new MESHDATA[Data.iNumMeshes];
	ZeroMemory(pMeshData, sizeof(MESHDATA) * Data.iNumMeshes);
	for (_uint iMeshIndex = 0; iMeshIndex < Data.iNumMeshes; ++iMeshIndex)
	{
		/* Read szMesh Name*/
		_uint iNameLength = { 0 };
		ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, pMeshData[iMeshIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

		/* Read iMaterialIndex */
		ReadFile(hFile, &pMeshData[iMeshIndex].iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

		/* Read iNumNonAnimVertices */
		ReadFile(hFile, &pMeshData[iMeshIndex].iNumNonAnimVertices, sizeof(_uint), &dwByte, nullptr);

		/* Read iNumAnimVertices */
		ReadFile(hFile, &pMeshData[iMeshIndex].iNumAnimVertices, sizeof(_uint), &dwByte, nullptr);

		/* Read iNumIndices */
		ReadFile(hFile, &pMeshData[iMeshIndex].iNumIndices, sizeof(_uint), &dwByte, nullptr);

		/* Read iNumMeshBones */
		ReadFile(hFile, &pMeshData[iMeshIndex].iNumMeshBones, sizeof(_uint), &dwByte, nullptr);

		/* Read pBoneIndices */
		pMeshData[iMeshIndex].pBoneIndices = new _uint[pMeshData[iMeshIndex].iNumMeshBones];
		ZeroMemory(pMeshData[iMeshIndex].pBoneIndices, sizeof(_uint) * pMeshData[iMeshIndex].iNumMeshBones);
		ReadFile(hFile, pMeshData[iMeshIndex].pBoneIndices, sizeof(_uint) * pMeshData[iMeshIndex].iNumMeshBones, &dwByte, nullptr);

		/* Read pNonAnimVertices */
		VTXMESH* pNonAnimVertices = { nullptr };
		if (0 < pMeshData[iMeshIndex].iNumNonAnimVertices)
		{
			pNonAnimVertices = new VTXMESH[pMeshData[iMeshIndex].iNumNonAnimVertices];
			ZeroMemory(pNonAnimVertices, sizeof(VTXMESH) * pMeshData[iMeshIndex].iNumNonAnimVertices);
			ReadFile(hFile, pNonAnimVertices, sizeof(VTXMESH) * pMeshData[iMeshIndex].iNumNonAnimVertices, &dwByte, nullptr);
		}

		/* Read pAnimVertices */
		VTXANIMMESH* pAnimVertices = { nullptr };
		if (0 < pMeshData[iMeshIndex].iNumAnimVertices)
		{
			pAnimVertices = new VTXANIMMESH[pMeshData[iMeshIndex].iNumAnimVertices];
			ZeroMemory(pAnimVertices, sizeof(VTXANIMMESH) * pMeshData[iMeshIndex].iNumAnimVertices);
			ReadFile(hFile, pAnimVertices, sizeof(VTXANIMMESH) * pMeshData[iMeshIndex].iNumAnimVertices, &dwByte, nullptr);
		}

		/* Read pIndices */
		_ulong* pIndices = new _ulong[pMeshData[iMeshIndex].iNumIndices];
		ZeroMemory(pIndices, sizeof(_ulong) * pMeshData[iMeshIndex].iNumIndices);
		ReadFile(hFile, pIndices, sizeof(_ulong) * pMeshData[iMeshIndex].iNumIndices, &dwByte, nullptr);

		pMeshData[iMeshIndex].pNonAnimVertices = pNonAnimVertices;
		pMeshData[iMeshIndex].pAnimVertices = pAnimVertices;
		pMeshData[iMeshIndex].pIndices = pIndices;
	}

	Data.pAnimations = pAnimation;
	Data.pMaterialPaths = pMaterialPath;
	Data.pBoneDatas = pBoneData;
	Data.pMeshDatas = pMeshData;

	CloseHandle(hFile);
}

HRESULT CFileInfo::Load(const string& strFilePath, OUT FILEDATA& OutData)
{
	HANDLE hFile = CreateFileA(strFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = { 0 };

	/* 1. Terrain을 불러온다. */
	_uint iXCount = { 0 }, iZCount = { 0 };

	/* Read iXCount */
	ReadFile(hFile, &iXCount, sizeof(_uint), &dwByte, nullptr);
	OutData.iXCount = iXCount;

	/* Read iZCount */
	ReadFile(hFile, &iZCount, sizeof(_uint), &dwByte, nullptr);
	OutData.iZCount = iZCount;

	/* Read pPositions */
	OutData.pPositions = new _float3[iXCount * iZCount];
	ZeroMemory(OutData.pPositions, sizeof(_float3) * iXCount * iZCount);

	for (_uint i = 0; i < iXCount * iZCount; ++i)
		ReadFile(hFile, &OutData.pPositions[i], sizeof(_float3), &dwByte, nullptr);

	/* 2. 전체 3D 게임오브젝트들을 불러온다. */
	_uint iNumObjects = { 0 };
	ReadFile(hFile, &iNumObjects, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNumObjects; ++i)
	{
		MODELDATA Data;
		ZeroMemory(&Data, sizeof(MODELDATA));

		/* Read szTag */
		_uint iTagLength = { 0 };
		ReadFile(hFile, &iTagLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, Data.szObjectTag, sizeof(_tchar) * iTagLength, &dwByte, nullptr);

		/* Read TransformMatrix */
		ReadFile(hFile, &Data.TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);

		/* Read vAngle */
		ReadFile(hFile, &Data.vAngle, sizeof(_float3), &dwByte, nullptr);

		/* Read Model Binary Datas */
		Read_BinData(hFile, Data.BinaryData, dwByte);

		OutData.vecModelData.push_back(Data);
	}

	/* 3. Monster 불러오기 */
	ReadFile(hFile, &iNumObjects, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNumObjects; ++i)
	{
		MODELDATA Data;
		ZeroMemory(&Data, sizeof(MODELDATA));

		/* Read szTag */
		_uint iTagLength = { 0 };
		ReadFile(hFile, &iTagLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, Data.szObjectTag, sizeof(_tchar) * iTagLength, &dwByte, nullptr);

		/* Read TransformMatrix */
		ReadFile(hFile, &Data.TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);

		/* Read vAngle */
		ReadFile(hFile, &Data.vAngle, sizeof(_float3), &dwByte, nullptr);

		OutData.vecMonsterData.push_back(Data);
	}

	/* 4. Player 불러오기*/
	ReadFile(hFile, &OutData.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
	ReadFile(hFile, &OutData.vAngle, sizeof(_float3), &dwByte, nullptr);

	/* 3. 카메라를 불러온다. */

	/* 4. UI를 불러온다. */

	CloseHandle(hFile);

	return S_OK;
}

void CFileInfo::Read_BinData(HANDLE hFile, MODEL_BINARYDATA& Data, _ulong dwByte)
{
	ZeroMemory(&Data, sizeof Data);

	_uint iPathLength = { 0 };
	/*========== BINARY DATAS ===========*/
	/* Read szTag */
	ReadFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, Data.szTag, sizeof(_tchar) * iPathLength, &dwByte, nullptr);

	/* Read iNumMeshes */
	ReadFile(hFile, &Data.iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	/* Read iNumMaterials */
	ReadFile(hFile, &Data.iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	/* Read pMaterialPaths */
	MATERIALPATH* pMaterialPath = { nullptr };
	if (0 < Data.iNumMaterials)
	{
		pMaterialPath = new MATERIALPATH[Data.iNumMaterials];
		ZeroMemory(pMaterialPath, sizeof(MATERIALPATH) * Data.iNumMaterials);
	}
	_uint iTexturePathlength = { 0 };
	for (_uint iMaterialIndex = 0; iMaterialIndex < Data.iNumMaterials; ++iMaterialIndex)
	{
		for (_uint iTextureIndex = 0; iTextureIndex < 21; ++iTextureIndex)
		{
			ReadFile(hFile, &iTexturePathlength, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, pMaterialPath[iMaterialIndex].szMaterialTexturePath[iTextureIndex],
				sizeof(_tchar) * iTexturePathlength, &dwByte, nullptr);
		}
	}

	/* Read iNumBones */
	ReadFile(hFile, &Data.iNumBones, sizeof(_uint), &dwByte, nullptr);

	/*========== BONE DATAS ===========*/
	BONEDATA* pBoneData = { nullptr };
	if (0 < Data.iNumBones)
	{
		pBoneData = new BONEDATA[Data.iNumBones];
		ZeroMemory(pBoneData, sizeof(BONEDATA) * Data.iNumBones);
	}

	for (_uint iBoneIndex = 0; iBoneIndex < Data.iNumBones; ++iBoneIndex)
	{
		/* Read szName_Bone */
		_uint iNameLength = { 0 };
		ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, pBoneData[iBoneIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

		/* Read iParentIdx */
		ReadFile(hFile, &pBoneData[iBoneIndex].iParentIdx, sizeof(_uint), &dwByte, nullptr);

		/* Read iIndex */
		ReadFile(hFile, &pBoneData[iBoneIndex].iIndex, sizeof(_uint), &dwByte, nullptr);

		/* Read TransformationMatrix */
		ReadFile(hFile, &pBoneData[iBoneIndex].TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

		/* Read OffsetMatrix */
		ReadFile(hFile, &pBoneData[iBoneIndex].OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);
	}

	/* Read iNumAnimations */
	ReadFile(hFile, &Data.iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	/*========== ANIMATION DATAS ===========*/
	ANIMATIONDATA* pAnimation = { nullptr };
	if (0 < Data.iNumAnimations)
	{
		pAnimation = new ANIMATIONDATA[Data.iNumAnimations];
		ZeroMemory(pAnimation, sizeof(ANIMATIONDATA) * Data.iNumAnimations);
	}

	for (_uint iAnimIndex = 0; iAnimIndex < Data.iNumAnimations; ++iAnimIndex)
	{
		/* Read szName */
		_uint iNameLength = strlen(pAnimation[iAnimIndex].szName) + 1;
		ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, pAnimation[iAnimIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

		/* Read Duration */
		ReadFile(hFile, &pAnimation[iAnimIndex].Duration, sizeof(_double), &dwByte, nullptr);

		/* Read TickPerSec */
		ReadFile(hFile, &pAnimation[iAnimIndex].TickPerSec, sizeof(_double), &dwByte, nullptr);

		/* Read iNumChannels */
		ReadFile(hFile, &pAnimation[iAnimIndex].iNumChannels, sizeof(_uint), &dwByte, nullptr);

		/* Read bIsLoop */
		ReadFile(hFile, &pAnimation[iAnimIndex].bIsLoop, sizeof(_bool), &dwByte, nullptr);

		/* Read bIsFollowAnimation */
		ReadFile(hFile, &pAnimation[iAnimIndex].bIsFollowAnimation, sizeof(_bool), &dwByte, nullptr);

		/* Read iNumRanges */
		ReadFile(hFile, &pAnimation[iAnimIndex].iNumRanges, sizeof(_uint), &dwByte, nullptr);

		/*===== TIMERANGE DATAS ======*/
		TIMERANGE* pTimeRange = { nullptr };
		if (0 < pAnimation[iAnimIndex].iNumRanges)
		{
			pTimeRange = new TIMERANGE[pAnimation[iAnimIndex].iNumRanges];
			ZeroMemory(pTimeRange, sizeof(TIMERANGE) * pAnimation[iAnimIndex].iNumRanges);
		}
		for (_uint iRangeIndex = 0; iRangeIndex < pAnimation[iAnimIndex].iNumRanges; ++iRangeIndex)
		{
			for (auto& Type : pTimeRange[iRangeIndex].arrTypes)
				Type = OBSERVERTYPE::TYPE_END;

			/* Read arrTypes */
			ReadFile(hFile, pTimeRange[iRangeIndex].arrTypes, sizeof(OBSERVERTYPE) * OBSERVERTYPE::TYPE_END, &dwByte, nullptr);

			/* Read fStartPoint */
			ReadFile(hFile, &pTimeRange[iRangeIndex].fStartPoint, sizeof(_float), &dwByte, nullptr);

			/* Read fEndPoint */
			ReadFile(hFile, &pTimeRange[iRangeIndex].fEndPoint, sizeof(_float), &dwByte, nullptr);
		}

		/*========== CHANNEL DATAS ===========*/
		CHANNELDATA* pChannel = { nullptr };
		if (0 < pAnimation[iAnimIndex].iNumChannels)
		{
			pChannel = new CHANNELDATA[pAnimation[iAnimIndex].iNumChannels];
			ZeroMemory(pChannel, sizeof(CHANNELDATA) * pAnimation[iAnimIndex].iNumChannels);
		}

		for (_uint iChannelIndex = 0; iChannelIndex < pAnimation[iAnimIndex].iNumChannels; ++iChannelIndex)
		{
			/* Read szName */
			iNameLength = strlen(pChannel[iChannelIndex].szName) + 1;
			ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, pChannel[iChannelIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

			/* Read iNumKeyFrames */
			ReadFile(hFile, &pChannel[iChannelIndex].iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);

			/* Read KeyFrame */
			KEYFRAME* pKeyFrame = { nullptr };
			if (0 < pChannel[iChannelIndex].iNumKeyFrames)
			{
				pKeyFrame = new KEYFRAME[pChannel[iChannelIndex].iNumKeyFrames];
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME) * pChannel[iChannelIndex].iNumKeyFrames);
			}

			ReadFile(hFile, pKeyFrame, sizeof(KEYFRAME) * pChannel[iChannelIndex].iNumKeyFrames, &dwByte, nullptr);
			pChannel[iChannelIndex].pKeyFrames = pKeyFrame;
		}
		pAnimation[iAnimIndex].pTimeRanges = pTimeRange;
		pAnimation[iAnimIndex].pChannels = pChannel;
	}

	/*========== MESH DATAS ===========*/
	MESHDATA* pMeshData = new MESHDATA[Data.iNumMeshes];
	ZeroMemory(pMeshData, sizeof(MESHDATA) * Data.iNumMeshes);
	for (_uint iMeshIndex = 0; iMeshIndex < Data.iNumMeshes; ++iMeshIndex)
	{
		/* Read szMesh Name*/
		_uint iNameLength = { 0 };
		ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, pMeshData[iMeshIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

		/* Read iMaterialIndex */
		ReadFile(hFile, &pMeshData[iMeshIndex].iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

		/* Read iNumNonAnimVertices */
		ReadFile(hFile, &pMeshData[iMeshIndex].iNumNonAnimVertices, sizeof(_uint), &dwByte, nullptr);

		/* Read iNumAnimVertices */
		ReadFile(hFile, &pMeshData[iMeshIndex].iNumAnimVertices, sizeof(_uint), &dwByte, nullptr);

		/* Read iNumIndices */
		ReadFile(hFile, &pMeshData[iMeshIndex].iNumIndices, sizeof(_uint), &dwByte, nullptr);

		/* Read iNumMeshBones */
		ReadFile(hFile, &pMeshData[iMeshIndex].iNumMeshBones, sizeof(_uint), &dwByte, nullptr);

		/* Read pBoneIndices */
		pMeshData[iMeshIndex].pBoneIndices = new _uint[pMeshData[iMeshIndex].iNumMeshBones];
		ZeroMemory(pMeshData[iMeshIndex].pBoneIndices, sizeof(_uint) * pMeshData[iMeshIndex].iNumMeshBones);
		ReadFile(hFile, pMeshData[iMeshIndex].pBoneIndices, sizeof(_uint) * pMeshData[iMeshIndex].iNumMeshBones, &dwByte, nullptr);

		/* Read pNonAnimVertices */
		VTXMESH* pNonAnimVertices = { nullptr };
		if (0 < pMeshData[iMeshIndex].iNumNonAnimVertices)
		{
			pNonAnimVertices = new VTXMESH[pMeshData[iMeshIndex].iNumNonAnimVertices];
			ZeroMemory(pNonAnimVertices, sizeof(VTXMESH) * pMeshData[iMeshIndex].iNumNonAnimVertices);
			ReadFile(hFile, pNonAnimVertices, sizeof(VTXMESH) * pMeshData[iMeshIndex].iNumNonAnimVertices, &dwByte, nullptr);
		}

		/* Read pAnimVertices */
		VTXANIMMESH* pAnimVertices = { nullptr };
		if (0 < pMeshData[iMeshIndex].iNumAnimVertices)
		{
			pAnimVertices = new VTXANIMMESH[pMeshData[iMeshIndex].iNumAnimVertices];
			ZeroMemory(pAnimVertices, sizeof(VTXANIMMESH) * pMeshData[iMeshIndex].iNumAnimVertices);
			ReadFile(hFile, pAnimVertices, sizeof(VTXANIMMESH) * pMeshData[iMeshIndex].iNumAnimVertices, &dwByte, nullptr);
		}

		/* Read pIndices */
		_ulong* pIndices = new _ulong[pMeshData[iMeshIndex].iNumIndices];
		ZeroMemory(pIndices, sizeof(_ulong) * pMeshData[iMeshIndex].iNumIndices);
		ReadFile(hFile, pIndices, sizeof(_ulong) * pMeshData[iMeshIndex].iNumIndices, &dwByte, nullptr);

		pMeshData[iMeshIndex].pNonAnimVertices = pNonAnimVertices;
		pMeshData[iMeshIndex].pAnimVertices = pAnimVertices;
		pMeshData[iMeshIndex].pIndices = pIndices;
	}

	Data.pAnimations = pAnimation;
	Data.pMaterialPaths = pMaterialPath;
	Data.pBoneDatas = pBoneData;
	Data.pMeshDatas = pMeshData;
}

wstring CFileInfo::strToWStr(const string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

string CFileInfo::wstrToStr(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}