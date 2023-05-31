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
	// �����θ� ����η� �����ϴ� �Լ�
	static wstring	ConvertRelatePath(wstring strFullPath);
	static void		Extraction_Texture(const std::wstring& wstrPath, std::list<IMGPATH*>& rPathInfoList);
    static _int		DirFileCnt(const std::wstring& wstrPath);

    // char wchar_t ��ȯ �Լ�
    static _tchar* strToWStr(const _char* str);
    static _char* wstrToStr(const _tchar* wstr);
};

END
