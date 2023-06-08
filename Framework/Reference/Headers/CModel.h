#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	const _uint& Get_NumMeshes() const {
		return m_iNumMeshes;
	}

public:
	virtual HRESULT Initialize_Prototype(const wstring & wstrFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(CComponent * pOwner, void* pArg) override;

	virtual HRESULT Render(const _uint& iMeshIndex);

public:
	HRESULT Bind_Material(class CShader* pShader, const string strTypename, const _uint & iMeshIndex, aiTextureType eTextureType);

private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer m_Importer;

private: /* For.Mesh */
	_uint m_iNumMeshes = { 0 };
	vector<class CMesh*> m_vecMeshes;

private: /* For.Material */
	_uint m_iNumMaterials = { 0 };
	vector<MESHMATERIAL> m_vecMaterials;

private:
	HRESULT Ready_Meshes(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const wstring & wstrFilePath);

public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& wstrFilePath, _fmatrix PivotMatrix);
	virtual CModel* Clone(CComponent * pOwner, void* pArg) override;
	virtual void Free() override;
};

END