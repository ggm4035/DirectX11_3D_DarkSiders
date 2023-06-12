#pragma once

#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CFileInfo final : public CBase
{
	DECLARE_SINGLETON(CFileInfo)

private:
	explicit CFileInfo();
	virtual ~CFileInfo() = default;

public:
	// 절대경로를 상대경로로 변경하는 함수
	wstring	ConvertRelatePath(wstring strFullPath);
	HRESULT Extraction_Data(const string& strPath, const _char* pExt, OUT list<string>& FilePathList);

public:
	void ReadModels(const string& strFileName, OUT list<string>& FilePathList, OUT vector<MODEL_BINARYDATA>& vecData);

public:
    // string < - > wstring 변환 함수
	wstring strToWStr(string& str);
	string wstrToStr(wstring& wstr);

public:
	virtual void Free() override {}
};

END
