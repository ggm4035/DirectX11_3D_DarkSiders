#pragma once

#include "CBase.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	explicit CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize();
	void Invalidate_TransformationMatrix();

private:
	_char m_szName[MAX_PATH] = { "" };
	_double m_Duration = { 0.0 };
	_double m_TickPerSec = { 0.0 };
	_double m_TimeAcc = { 0.0 };

private:
	vector<class CChannel*> m_vecChannels;

public:
	static CAnimation* Create();
	virtual void Free() override;
};

END