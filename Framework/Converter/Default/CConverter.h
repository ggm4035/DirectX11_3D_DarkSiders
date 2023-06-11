#pragma once

#include "Converter_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CFileInfo;
class CGameInstance;
END

class CConverter final : public CBase
{
private:
	explicit CConverter();
	virtual ~CConverter() = default;

public:
	HRESULT Initialize();
	void Tick();

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:
	Assimp::Importer m_Importer;
	const aiScene* m_pAIScene = { nullptr };

private: /* For.Store Data_NonAnimModels */
	list<string> m_FilePathList;
	vector<NONANIM_MODEL_BINARYDATA> m_vecDatas;

	string m_strName = { "" };
	_uint m_iMaterialIndex = { 0 };
	_uint m_iNumMaterials = { 0 };
	_uint m_iNumVertices = { 0 };
	_uint m_iNumIndices = { 0 };
	_uint m_iNumMeshes = { 0 };

private: /* FilePathFinder */
	void WriteNonAnimModels(const string& strFileName);
	void WriteAnimModels(const string& strFileName);

	void ReadNonAnimModels(const string& strFileName);
	void ReadAnimModels(const string& strFileName);

private:
	void ConvertBinary_NonAnimModel();
	HRESULT Ready_Meshes_NonAnimModel(_fmatrix PivotMatrix, OUT MESHDATA* pData);
	HRESULT Ready_Materials_NonAnimModel(const string& strFilePath, OUT MESHDATA* pData);

public:
	static CConverter* Create();
	virtual void Free() override;
};

