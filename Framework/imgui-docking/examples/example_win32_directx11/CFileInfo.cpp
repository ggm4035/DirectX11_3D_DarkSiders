#include "CFileInfo.h"

wstring CFileInfo::ConvertRelatePath(wstring strFullPath)
{
	_tchar szRelativePath[MAX_PATH] = L"";
	_tchar szCurDirPath[MAX_PATH] = L"";

	// ���� ��θ� �������� �Լ�
	GetCurrentDirectory(MAX_PATH, szCurDirPath);

	// ���� ���(szCurDirPath) ���� ���ڷ� ���� ���(strFullPath) ���� ���� ����� ���� �ϴ� �Լ�
	// ��, ���� ����̺� �������� ����(C:, D:...)
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
