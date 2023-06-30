#pragma once

#include "CModel.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
	friend class CModel;

private:
	explicit CAnimation();
	explicit CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	_bool isLoop() const {
		return m_isLoop;
	}
	_bool isLerped() const {
		return m_isLerped;
	}
	void Pause_Animation() {
		m_isPause = true;
	}
	void Play_Animation() {
		m_isPause = false;
	}
	_uint Get_RootIndex() {
		return m_iRootBoneIndex;
	}

	void Bind_LerpAnimation(CAnimation* pLerpAnimation);

public:
	HRESULT Initialize(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, const _double& TimeDelta);
	void Invalidate_NotMoveTransformationMatrix(CModel::BONES& Bones, const _double& TimeDelta);

	void Reset_Animation();

private:
	_char m_szName[MAX_PATH] = { "" };
	_double m_Duration = { 0.0 };
	_double m_TickPerSec = { 0.0 };
	_double m_TimeAcc = { 0.0 };

	_double m_LerpDuration = { 0.2 };

	_bool m_isFinished = { false };
	_bool m_isLerped = { true };
	_bool m_isLoop = { false };
	_bool m_isBlock = { false };
	_bool m_isPause = { false };

	_uint m_iBlockIndex = { 0 };
	_uint m_iRootBoneIndex = { 0 };

private:
	CAnimation* m_pLerpAnimation = { nullptr };

	_uint m_iNumChannels = { 0 };
	vector<class CChannel*> m_vecChannels;
	vector<_uint> m_vecChannelCurrentKeyFrames;
	vector<_uint> m_vecChannelBlockKeyFrames;

private:
	HRESULT Ready_Channels(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones);
	_bool Lerp_Animation(CModel::BONES& Bones, const _double& TimeDelta);

public:
	static CAnimation* Create(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;

#if defined(_USE_IMGUI) || defined(_DEBUG)

public: /* !!! Warrning !!! Only Tool */
	const _uint& Get_MaxKeyFrames() const {
		return m_iMaxNumFrames;
	}

	vector<KEYFRAME>& Get_KeyFrames();
	vector<KEYFRAME>& Get_RootKeyFrames();

	const _uint& Get_CurrentKeyFrameIndex() const {
		return m_vecChannelCurrentKeyFrames[m_iMaxFramesIndex];
	}

	const _uint& Get_MaxRootKeyFrames() const {
		return m_iMaxNumRootFrames;
	}
	const _uint& Get_CurrentRootKeyFrameIndex() const {
		return m_vecChannelCurrentKeyFrames[m_iRootBoneIndex];
	}
	void Set_KeyFrame(const _uint& iIndex);
	void Set_RootKeyFrame(const _uint& iIndex);

private:
	_uint m_iMaxNumRootFrames = { 0 };
	_uint m_iMaxNumFrames = { 0 };
	_uint m_iMaxFramesIndex = { 0 };

#endif
};

END