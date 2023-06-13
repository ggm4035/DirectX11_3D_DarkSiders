#pragma once

#include "CBase.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	explicit CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const CHANNELDATA& ChannelData);
	void Invalidate_TransformationMatrix(const _double& TimeAcc);

private:
	_char m_szName[MAX_PATH] = { "" };
	_uint m_iCurrentKeyFrame = { 0 };

private:
	_uint m_iNumKeyFrames = { 0 };
	vector<KEYFRAME> m_vecKeyFrames;

private:
	HRESULT Ready_KeyFrames(const CHANNELDATA& ChannelData);

public:
	static CChannel* Create(const CHANNELDATA& ChannelData);
	virtual void Free() override;
};

END