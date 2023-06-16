#pragma once

#include "CModel.h"

BEGIN(Engine)

class CChannel final : public CBase
{
	friend class CModel;

private:
	explicit CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const CHANNELDATA& ChannelData, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(const CModel::BONES& Bones, const _double& TimeAcc, _uint* pCurrentKeyFrame);

private:
	_char m_szName[MAX_PATH] = { "" };
	_uint m_iBoneIndex = { 0 };

private:
	_uint m_iNumKeyFrames = { 0 };
	vector<KEYFRAME> m_vecKeyFrames;

public:
	static CChannel* Create(const CHANNELDATA& ChannelData, const CModel::BONES& Bones);
	virtual void Free() override;
};

END