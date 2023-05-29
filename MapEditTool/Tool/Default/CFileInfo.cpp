#include "pch.h"
#include "CFileInfo.h"

CString CFileInfo::ConvertRelatePath(CString strFullPath)
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
		strFullPath.GetString(),
		FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);
}

void CFileInfo::Extraction_Texture(const std::wstring& wstrPath, std::list<IMGPATH*>& rPathInfoList)
{
	CFileFind Find;

	wstring wstrFilePath = wstrPath + L"\\*.*";

	BOOL bContinue = Find.FindFile(wstrFilePath.c_str()); // 파일을 찾는다.

	while (bContinue) // 파일이 있으면
	{
		bContinue = Find.FindNextFileW(); // 첫번째 파일에 드가서 파일이 있는지 체크

		if (Find.IsDots())
			continue;

		else if (Find.IsDirectory())
		{
			Extraction_Texture(wstring(Find.GetFilePath()), rPathInfoList);
		}

		else
		{
			if (Find.IsSystem())
				continue;

			_tchar szPathBuf[MAX_PATH] = { L"" };
			lstrcpy(szPathBuf, Find.GetFilePath().GetString());

			_tchar szExt[32] = { L"" };
			_wsplitpath_s(szPathBuf, nullptr, 0, nullptr, 0, nullptr, 0, szExt, 32);
			if (false != lstrcmp(szExt, L".png"))
				continue;

			_tchar szTextureName[MAX_PATH] = { L"" };
			_wsplitpath_s(szPathBuf, nullptr, 0, nullptr, 0, szTextureName, MAX_PATH, nullptr, 0);
			wstring wstrTextureName = szTextureName;
			wstrTextureName.substr(0, wstrTextureName.size() - 1) + L"%d.png";

			IMGPATH* pImgPath = new IMGPATH;

			PathRemoveFileSpec(szPathBuf);
			pImgPath->iCount = DirFileCnt(szPathBuf);

			PathCombine(szPathBuf, szPathBuf, wstrTextureName.c_str());
			pImgPath->wstrPath = ConvertRelatePath(szPathBuf);

			PathRemoveFileSpec(szPathBuf);
			pImgPath->wstrObjKey = PathFindFileName(szPathBuf);

			rPathInfoList.push_back(pImgPath);

			bContinue = FALSE;
		}
	}
}

_int CFileInfo::DirFileCnt(const std::wstring& wstrPath)
{
	wstring wstrFilePath = wstrPath + L"\\*.*";

	CFileFind Find;
	BOOL bContinue = Find.FindFile(wstrFilePath.c_str());
	
	int iFileCnt = 0;

	while (bContinue)
	{
		bContinue = Find.FindNextFileW();

		if(Find.IsDots())
			continue;

		if(Find.IsSystem())
			continue;

		++iFileCnt;
	}

	return iFileCnt;
}
