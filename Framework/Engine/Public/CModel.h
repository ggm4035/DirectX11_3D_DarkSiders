#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
	typedef vector<class CBone*> BONES;

private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	const _uint& Get_NumMeshes() const {
		return m_pModelDataDesc->iNumMeshes;
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const NONANIM_MODEL_BINARYDATA& ModelData, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(CComponent * pOwner, void* pArg) override;

	virtual HRESULT Render(const _uint& iMeshIndex);

public:
	void Play_Animation();

public:
	HRESULT Bind_Material(class CShader* pShader, const string& strTypename, const _uint & iMeshIndex, TEXTURETYPE eTextureType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const string& strTypename, const _uint& iMeshIndex);

private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer m_Importer;

private: /* For.Data */
	const NONANIM_MODEL_BINARYDATA* m_pModelDataDesc;

private: /* For.Mesh */
	vector<class CMesh*> m_vecMeshes;

private: /* For.Material */
	vector<MESHMATERIAL> m_vecMaterials;

private: /* For.Bone */
	vector<class CBone*> m_vecBones;

private:
	HRESULT Ready_Meshes(TYPE eModelType, _fmatrix PivotMatrix);
	HRESULT Ready_Materials();
	HRESULT Ready_Bones(aiNode* pAINode, class CBone* pParent);

public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, 
		TYPE eModelType, const NONANIM_MODEL_BINARYDATA& ModelData, _fmatrix PivotMatrix);
	virtual CModel* Clone(CComponent * pOwner, void* pArg) override;
	virtual void Free() override;
};

END