#pragma once

#include "CBase.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	explicit CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const ANIMATIONDATA& AnimationData);
	void Invalidate_TransformationMatrix(const _double& TimeDelta);

private:
	_char m_szName[MAX_PATH] = { "" };
	_double m_Duration = { 0.0 };
	_double m_TickPerSec = { 0.0 };
	_double m_TimeAcc = { 0.0 };

private:
	_uint m_iNumChannels = { 0 };
	vector<class CChannel*> m_vecChannels;

private:
	HRESULT Ready_Channels(const ANIMATIONDATA& AnimationData);

public:
	static CAnimation* Create(const ANIMATIONDATA& AnimationData);
	virtual void Free() override;
};

END