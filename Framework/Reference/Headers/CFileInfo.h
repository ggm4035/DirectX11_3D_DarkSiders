#pragma once

#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CFileInfo final : public CBase
{
	DECLARE_SINGLETON(CFileInfo)

public:
    typedef	struct tagTexturePath
    {
        wstring			wstrObjKey = { L"" };
        wstring			wstrPath = { L"" };
        int				iCount = { 0 };
    }IMGPATH;

private:
	explicit CFileInfo();
	virtual ~CFileInfo() = default;

public:
	// �����θ� ����η� �����ϴ� �Լ�
	wstring	ConvertRelatePath(wstring strFullPath);
	void	Extraction_Texture(const std::wstring& wstrPath, std::list<IMGPATH*>& rPathInfoList);
	_int	DirFileCnt(const std::wstring& wstrPath);

public:
    // char wchar_t ��ȯ �Լ�
	wstring strToWStr(string& str);
	string wstrToStr(wstring& wstr);

public:
	static CFileInfo* Create();
	virtual void Free() override {}
};

END
