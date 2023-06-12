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

private: 
	list<string> m_FilePathList;
	vector<MODEL_BINARYDATA> m_vecDatas;

	string m_strName = { "" };
	_uint m_iMaterialIndex = { 0 };
	_uint m_iNumMaterials = { 0 };
	_uint m_iNumVertices = { 0 };
	_uint m_iNumIndices = { 0 };
	_uint m_iNumMeshes = { 0 };
	_uint m_iNumAnimations = { 0 };

private:
	vector<BONEDATA> m_vecBones;
	vector<_uint> m_vecBoneIndices;

private: /* FilePathFinder */
	void WriteModels(const string& strFileName);

private:
	void ConvertBinary_AnimModel();
	void ConvertBinary_NonAnimModel();
	HRESULT Ready_Bones(aiNode* pAINode, const _uint& iParentIdx);
	HRESULT Ready_Meshes_AnimModel(OUT MESHDATA* pData);
	HRESULT Ready_Materials(const string& strFilePath, MODEL_BINARYDATA& Data);
	HRESULT Ready_Meshes_NonAnimModel(_fmatrix PivotMatrix, OUT MESHDATA* pData);
	HRESULT Ready_Animation(MODEL_BINARYDATA& Data);

public:
	static CConverter* Create();
	void ResetData();
	virtual void Free() override;
};