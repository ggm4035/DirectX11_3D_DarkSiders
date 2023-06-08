#pragma once

#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CFileInfo final : public CBase
{
	DECLARE_SINGLETON(CFileInfo)

public:
	typedef struct tagFileInfoDesc
	{
		wstring wstrTag;
		wstring wstrFilePath;
	}FILEDESC;

private:
	explicit CFileInfo();
	virtual ~CFileInfo() = default;

public:
	// 절대경로를 상대경로로 변경하는 함수
	wstring	ConvertRelatePath(wstring strFullPath);
	void	Extraction_Texture(const std::wstring& wstrPath, OUT std::list<FILEDESC*>& rPathInfoList);
	void	Extraction_Mesh(const std::wstring& wstrPath, OUT std::list<FILEDESC*>& rPathList);

public:
    // char wchar_t 변환 함수
	wstring strToWStr(string& str);
	string wstrToStr(wstring& wstr);

public:
	virtual void Free() override {}
};

END
