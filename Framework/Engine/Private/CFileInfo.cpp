#include "CFileInfo.h"
#include <locale>
#include <codecvt>

IMPLEMENT_SINGLETON(CFileInfo)

CFileInfo::CFileInfo()
{
}

wstring CFileInfo::ConvertRelatePath(wstring strFullPath)
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

void CFileInfo::ReadNonAnimModels(const string& strFileName, OUT list<string>& FilePathList, OUT vector<NONANIM_MODEL_BINARYDATA>& vecData)
{
	FilePathList.clear();

	_char szFullPath[MAX_PATH] = { "" };
	GetCurrentDirectoryA(MAX_PATH, szFullPath);
	string strFullPath = szFullPath;

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
			FilePathList.push_back(szFilePath);

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
			ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
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

		vecData.push_back(Data);
	}
}

wstring CFileInfo::strToWStr(string& str)
{
	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.from_bytes(str);
}

string CFileInfo::wstrToStr(wstring& wstr)
{
	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(wstr);
}