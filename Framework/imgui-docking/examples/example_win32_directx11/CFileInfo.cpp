#include "CFileInfo.h"

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

void CFileInfo::Extraction_Texture(const std::wstring& wstrPath, std::list<IMGPATH*>& rPathInfoList)
{
}

_int CFileInfo::DirFileCnt(const std::wstring& wstrPath)
{
	return 0;
}

_tchar* CFileInfo::strToWStr(const _char* str)
{
    size_t iStrLength = strlen(str);
    size_t ineed = 0;
    _tchar* wStr = nullptr;
    mbstowcs_s(&ineed, wStr, iStrLength, str, _TRUNCATE);

    return wStr;
}

_char* CFileInfo::wstrToStr(const _tchar* wstr)
{
    return nullptr;
}
