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
