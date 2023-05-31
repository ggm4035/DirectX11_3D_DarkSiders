#pragma once

#include "IMGUI_Defines.h"
#include "CBase.h"

BEGIN(Imgui)

class CFileInfo
{
public:
    typedef	struct tagTexturePath
    {
        wstring			wstrObjKey = { L"" };
        wstring			wstrPath = { L"" };
        int				iCount = { 0 };
    }IMGPATH;

private:
	CFileInfo() = default;
	~CFileInfo() = default;

public:
	// 절대경로를 상대경로로 변경하는 함수
	static wstring	ConvertRelatePath(wstring strFullPath);
	static void		Extraction_Texture(const std::wstring& wstrPath, std::list<IMGPATH*>& rPathInfoList);
    static _int		DirFileCnt(const std::wstring& wstrPath);

    // char wchar_t 변환 함수
    static _tchar* strToWStr(const _char* str);
    static _char* wstrToStr(const _tchar* wstr);
};

END
