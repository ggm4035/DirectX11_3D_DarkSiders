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
