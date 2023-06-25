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

public:
	HRESULT Initialize(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, const _double& TimeDelta);

	void Reset_Animation();

private:
	_char m_szName[MAX_PATH] = { "" };
	_double m_Duration = { 0.0 };
	_double m_TickPerSec = { 0.0 };
	_double m_TimeAcc = { 0.0 };

	_bool m_isFinished = { false };
	_bool m_isLoop = { false };

private:
	_uint m_iNumChannels = { 0 };
	vector<class CChannel*> m_vecChannels;
	vector<_uint> m_vecChannelCurrentKeyFrames;

private:
	HRESULT Ready_Channels(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones);

public:
	static CAnimation* Create(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END