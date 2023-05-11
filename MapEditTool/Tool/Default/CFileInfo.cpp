#include "pch.h"
#include "CFileInfo.h"

CString CFileInfo::ConvertRelatePath(CString strFullPath)
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
		strFullPath.GetString(),
		FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);
}
