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
		return m_iNumMeshes;
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const MODEL_BINARYDATA& ModelData);
	virtual HRESULT Initialize(CComponent * pOwner, void* pArg) override;

	virtual HRESULT Render(const _uint& iMeshIndex);

public:
	void Play_Animation(const _double& TimeDelta);

public:
	HRESULT Bind_Material(class CShader* pShader, const string& strTypename, const _uint & iMeshIndex, TEXTURETYPE eTextureType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const string& strTypename, const _uint& iMeshIndex);

private: /* For.Mesh */
	_uint m_iNumMeshes = { 0 };
	vector<class CMesh*> m_vecMeshes;

private: /* For.Material */
	_uint m_iNumMaterials = { 0 };
	vector<MESHMATERIAL> m_vecMaterials;

private: /* For.Bone */
	vector<class CBone*> m_vecBones;

private: /* For.Animation */
	_uint m_iCurrentAnimIndex = { 0 };
	_uint m_iNumAnimations = { 0 };
	vector<class CAnimation*> m_vecAnimations;

private:
	HRESULT Ready_Meshes(const MODEL_BINARYDATA& ModelData, TYPE eModelType);
	HRESULT Ready_Materials(const MODEL_BINARYDATA& ModelData);
	HRESULT Ready_Bones(const MODEL_BINARYDATA& ModelData, class CBone* pParent);
	HRESULT Ready_Animations(const MODEL_BINARYDATA& ModelData);

public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, 
		TYPE eModelType, const MODEL_BINARYDATA& ModelData);
	virtual CModel* Clone(CComponent * pOwner, void* pArg) override;
	virtual void Free() override;
};

END