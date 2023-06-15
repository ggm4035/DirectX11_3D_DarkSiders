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
	wstring	ConvertRelatePath(const wstring& strFullPath);
	HRESULT Extraction_Data(const string& strPath, const _char* pExt, OUT list<string>& FilePathList);

public:
	void ReadModels(const string& strFilePath, OUT list<string>& FilePathList, OUT vector<MODEL_BINARYDATA>& vecData);
	HRESULT Load(const string& strFilePath, OUT list<FILEDATA>& OutData);

public:
    // string < - > wstring ��ȯ �Լ�
	wstring strToWStr(const string& str);
	string wstrToStr(const wstring& wstr);

public:
	virtual void Free() override {}
};

END
