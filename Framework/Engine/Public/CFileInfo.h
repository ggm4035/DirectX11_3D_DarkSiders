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
	wstring	ConvertRelatePath(const wstring& strFullPath);
	HRESULT Extraction_Data(const string& strPath, const _char* pExt, OUT list<string>& FilePathList);

public:
	void WriteModels(const string & strFilePath, const list<string>& FilePathList, const vector<MODEL_BINARYDATA>& vecData);
	void ReadModels(const string & strFilePath, OUT list<string>&FilePathList, OUT vector<MODEL_BINARYDATA>&vecData);
	void ReadModel(const string & strFilePath, OUT string& FilePath, OUT MODEL_BINARYDATA& Data);
	HRESULT Load(const string& strFilePath, OUT FILEDATA& OutData);

private:
	void Read_BinData(HANDLE hFile, MODEL_BINARYDATA & Data, _ulong dwByte);

public:
    // string < - > wstring 변환 함수
	wstring strToWStr(const string& str);
	string wstrToStr(const wstring& wstr);

public:
	virtual void Free() override {}
};

END
