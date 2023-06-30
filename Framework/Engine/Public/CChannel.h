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
	const KEYFRAME& Get_FirstKeyFrame() const {
		return m_vecKeyFrames.front();
	}

public:
	HRESULT Initialize(const CHANNELDATA& ChannelData, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(const CModel::BONES& Bones, const _double& TimeAcc, _uint* pCurrentKeyFrame);
	void Lerp_TransformationMatrix(const CModel::BONES& Bones, const _double& LerpDuration, const _double& TimeAcc, 
		const _uint& CurrentKeyFrame, const KEYFRAME& KeyFrame);
	void Invalidate_NotMoveTransformationMatrix(const CModel::BONES& Bones, const _double& TimeAcc, _uint* pCurrentKeyFrame, const _uint& iBlockIndex);

private:
	_char m_szName[MAX_PATH] = { "" };
	_uint m_iBoneIndex = { 0 };

private:
	_uint m_iNumKeyFrames = { 0 };
	vector<KEYFRAME> m_vecKeyFrames;

public:
	static CChannel* Create(const CHANNELDATA& ChannelData, const CModel::BONES& Bones);
	virtual void Free() override;

#if defined(_USE_IMGUI) || defined(_DEBUG)

	vector<KEYFRAME>& Get_KeyFrames() {
		return m_vecKeyFrames;
	}

#endif
};

END