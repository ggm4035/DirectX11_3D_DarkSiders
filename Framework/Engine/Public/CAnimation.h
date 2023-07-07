#pragma once

#include "Observer_Interfaces.h"
#include "CModel.h"

BEGIN(Engine)

class CAnimation final : public CBase, IObserver_Animation
{
	friend class CModel;

private:
	explicit CAnimation();
	explicit CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	vector<TIMERANGE> Get_TimeRanges() const {
		return m_vecTimeRange;
	}
	const _float& Get_TimeAcc() const {
		return m_TimeAcc;
	}
	_bool isFollowAnimation() const {
		return m_isFollowAnimation;
	}
	_bool isLoop() const {
		return m_isLoop;
	}
	_bool isLerped() const {
		return m_isLerped;
	}
	_bool isAbleChange() const {
		return m_isAbleChange;
	}

	void Bind_LerpAnimation(CAnimation* pLerpAnimation);

public:
	HRESULT Initialize(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, const _double& TimeDelta);

	void Reset_Animation();

public:
	virtual void Update_Observer(NOTIFYDESC* pNotifyDesc) override {
		m_isAbleChange = true;
	}

private:
	string m_strName = { "" };
	_double m_TimeAcc = { 0.0 };
	_double m_Duration = { 0.0 };
	_double m_TickPerSec = { 0.0 };

	_double m_LerpDuration = { 0.2 };

	_bool m_isLoop = { false };
	_bool m_isLerped = { true };
	_bool m_isBlock = { false };
	_bool m_isFinished = { false };
	_bool m_isAbleChange = { false };
	_bool m_isFollowAnimation = { true };

	_uint m_iBlockIndex = { 0 };
	_uint m_iRootBoneIndex = { 0 };

private:
	CAnimation* m_pLerpAnimation = { nullptr };

	_uint m_iNumChannels = { 0 };
	vector<TIMERANGE> m_vecTimeRange;
	vector<class CChannel*> m_vecChannels;
	vector<_uint> m_vecChannelBlockKeyFrames;
	vector<_uint> m_vecChannelCurrentKeyFrames;

private:
	HRESULT Ready_TimeRanges(const ANIMATIONDATA& AnimationData);
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

	void Pause_Animation() {
		m_isPause = true;
	}
	void Play_Animation() {
		m_isPause = false;
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
	_uint m_iMaxFramesIndex = { 0 };
	_uint m_iMaxNumFrames = { 0 };
	_bool m_isPause = { false };

#endif
};

END