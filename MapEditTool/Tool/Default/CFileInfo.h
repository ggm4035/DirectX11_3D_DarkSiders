#pragma once

#include "Tool_Defines.h"
#include "CBase.h"

BEGIN(Tool)

typedef	struct tagTexturePath
{
	wstring			wstrObjKey = { L"" };
	wstring			wstrPath = { L"" };
	int				iCount = { 0 };
}IMGPATH;

END

class CFileInfo
{
private:
	CFileInfo() = default;
	~CFileInfo() = default;

public:
	// �����θ� ����η� �����ϴ� �Լ�
	static CString	ConvertRelatePath(CString strFullPath);
	static void		Extraction_Texture(const std::wstring& wstrPath, std::list<IMGPATH*>& rPathInfoList); // .png �ؽ�ó ����
	static _int		DirFileCnt(const std::wstring& wstrPath);
};