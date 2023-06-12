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
	// �����θ� ����η� �����ϴ� �Լ�
	wstring	ConvertRelatePath(wstring strFullPath);
	HRESULT Extraction_Data(const string& strPath, const _char* pExt, OUT list<string>& FilePathList);

public:
	void ReadModels(const string& strFileName, OUT list<string>& FilePathList, OUT vector<MODEL_BINARYDATA>& vecData);

public:
    // string < - > wstring ��ȯ �Լ�
	wstring strToWStr(string& str);
	string wstrToStr(wstring& wstr);

public:
	virtual void Free() override {}
};

END
