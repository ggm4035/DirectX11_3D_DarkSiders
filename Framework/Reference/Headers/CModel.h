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
	_bool isLoopAnimation() const;
	_bool isFinishedAnimation() const;
	_bool isFollowAnimation() const;
	_bool isAbleChangeAnimation() const;

	const TYPE Get_Type() const {
		return m_eType;
	}

	_matrix Get_PivotMatrix() const {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

	const _uint& Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	const class CBone* Get_Bone(const _char* pBoneName);

public:
	void Change_Animation(const string& strTag);

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const MODEL_BINARYDATA& ModelData, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent * pOwner, void* pArg) override;
	virtual HRESULT Render(const _uint& iMeshIndex);

public:
	_vector ComputeAnimMovement(OUT _float3* pDirection = nullptr);
	void Play_Animation(const _double& TimeDelta, class CNavigation* pNavigation);
	void Pause_Animation();
	void RePlay_Animation();
	//HRESULT Setup_Notifys(const wstring& wstrFilePath);

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
	_uint m_iRootBoneIndex = { 0 };
	vector<class CBone*> m_vecBones;

private: /* For.Animation */
	class CAnimation* m_pCurrentAnimation = { nullptr };
	_uint m_iNumAnimations = { 0 };
	vector<class CAnimation*> m_vecAnimations;

private:
	_bool m_isFirst = { true };
	_bool m_isCloned = { false };
	TYPE m_eType = { TYPE_END };
	_float4x4 m_PivotMatrix;
	_float4x4 m_WorldMatrix;

private:
	HRESULT Ready_Meshes(const MODEL_BINARYDATA& ModelData, TYPE eModelType);
	HRESULT Ready_Materials(const MODEL_BINARYDATA& ModelData);
	HRESULT Ready_Bones(const MODEL_BINARYDATA& ModelData, class CBone* pParent);
	HRESULT Ready_Animations(const MODEL_BINARYDATA& ModelData);
	class CAnimation* Find_Animation(const string& strTag);

public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, 
		TYPE eModelType, const MODEL_BINARYDATA& ModelData, _fmatrix PivotMatrix);
	virtual CModel* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;

#if defined(_USE_IMGUI) || defined(_DEBUG)

public: /* !!! Warrning !!! Only Tool */
	const vector<class CMesh*>& Get_Meshes() const {
		return m_vecMeshes;
	}
	const _uint& Get_MaxNumKeyFrame() const;
	const _uint& Get_MaxRootKeyFrame() const;
	const _uint& Get_CurrentKeyFrameIndex() const;
	const _uint& Get_CurrentRootKeyFrameIndex() const;
	void Set_KeyFrame(const _uint& iIndex);
	void Set_RootKeyFrame(const _uint& iIndex);
	void Set_Translation(const _uint& iIndex, const _float3& vTranslation);
	void Set_KeyFrameTime(const _uint& iIndex, const _float& fTime);
	void Set_Duration(const _float& fDuration);
	void Set_TickPerSec(const _float& fTickPerSec);
	_float3 Get_Translation(const _uint& iIndex);
	_float Get_KeyFrameTime(const _uint& iIndex);
	_float Get_Duration();
	_float Get_TickPerSec();
	_bool isPause() const;

	vector<KEYFRAME>& Get_MaxKeyFrames(const _uint iAnimIndex);
	void Set_MaxKeyFrames(const vector<KEYFRAME>& vecKeyFrame);
	vector<ANIMATIONDATA> Get_AnimationDatas();
	HRESULT Set_Animation(const string& wstrTag, const ANIMATIONDATA& AnimData);
	HRESULT Add_Animation(const ANIMATIONDATA& AnimData);
	HRESULT Delete_Animation(const string& wstrTag);

#endif
};

END