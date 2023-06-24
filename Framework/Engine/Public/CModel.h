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
	const TYPE Get_Type() const {
		return m_eType;
	}

	const _uint& Get_NumMeshes() const {
		return m_iNumMeshes;
	}

public:
	void Set_AnimIndex(const _uint& iAnimIndex) {
		if (iAnimIndex >= m_iNumAnimations)
			return;
		m_iCurrentAnimIndex = iAnimIndex;
	}

public:/* !!! Warrning Only Tool !!! */
	const vector<class CMesh*>& Get_Meshes() const {
		return m_vecMeshes;
	}
	vector<ANIMATIONDATA> Get_AnimationDatas();
	HRESULT Set_Animation(_uint iIndex, const ANIMATIONDATA& AnimData);
	HRESULT Add_Animation(const ANIMATIONDATA& AnimData);
	HRESULT Delete_Animation(_uint iIndex);

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const MODEL_BINARYDATA& ModelData, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(const _uint& iLayerIndex, CComponent * pOwner, void* pArg) override;

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
	TYPE m_eType = { TYPE_END };
	_float4x4 m_PivotMatrix;

private:
	HRESULT Ready_Meshes(const MODEL_BINARYDATA& ModelData, TYPE eModelType);
	HRESULT Ready_Materials(const MODEL_BINARYDATA& ModelData);
	HRESULT Ready_Bones(const MODEL_BINARYDATA& ModelData, class CBone* pParent);
	HRESULT Ready_Animations(const MODEL_BINARYDATA& ModelData);

public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, 
		TYPE eModelType, const MODEL_BINARYDATA& ModelData, _fmatrix PivotMatrix);
	virtual CModel* Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END