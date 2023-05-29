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
	// 절대경로를 상대경로로 변경하는 함수
	static CString	ConvertRelatePath(CString strFullPath);
	static void		Extraction_Texture(const std::wstring& wstrPath, std::list<IMGPATH*>& rPathInfoList); // .png 텍스처 추출
	static _int		DirFileCnt(const std::wstring& wstrPath);
};