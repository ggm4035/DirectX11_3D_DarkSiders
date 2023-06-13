#include "CChannel.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const CHANNELDATA& ChannelData)
{
	strcpy_s(m_szName, ChannelData.szName);

	if (FAILED(Ready_KeyFrames(ChannelData)))
		return E_FAIL;

	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(const _double& TimeAcc)
{
	KEYFRAME LastKeyFrame = m_vecKeyFrames.back();

	_vector vScale;
	_vector vRotation;
	_vector vTranslation;

	if (TimeAcc >= LastKeyFrame.Time)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMLoadFloat3(&LastKeyFrame.vTranslation);
	}

	else /* 현재 존재하는 키프레임의 상태를 좌우 키프레임정보를 이용하여 선형보간한다. */
	{
		if (TimeAcc >= m_vecKeyFrames[m_iCurrentKeyFrame + 1].Time)
			++m_iCurrentKeyFrame;

		_double Ratio = (TimeAcc - m_vecKeyFrames[m_iCurrentKeyFrame].Time) /
			(m_vecKeyFrames[m_iCurrentKeyFrame + 1].Time - m_vecKeyFrames[m_iCurrentKeyFrame].Time);

		_vector SourScale = XMLoadFloat3(&m_vecKeyFrames[m_iCurrentKeyFrame].vScale);
		_vector SourRotation = XMLoadFloat4(&m_vecKeyFrames[m_iCurrentKeyFrame].vRotation);
		_vector SourTranslation = XMLoadFloat3(&m_vecKeyFrames[m_iCurrentKeyFrame].vTranslation);

		_vector DestScale = XMLoadFloat3(&m_vecKeyFrames[m_iCurrentKeyFrame + 1].vScale);
		_vector DestRotation = XMLoadFloat4(&m_vecKeyFrames[m_iCurrentKeyFrame + 1].vRotation);
		_vector DestTranslation = XMLoadFloat3(&m_vecKeyFrames[m_iCurrentKeyFrame + 1].vTranslation);

		vScale = XMVectorLerp(SourScale, DestScale, Ratio);
		vRotation = XMVectorLerp(SourRotation, DestRotation, Ratio);
		vTranslation = XMVectorLerp(SourTranslation, DestTranslation, Ratio);
	}

	vTranslation.m128_f32[3] = 1.f;
	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
}

HRESULT CChannel::Ready_KeyFrames(const CHANNELDATA& ChannelData)
{
	m_iNumKeyFrames = ChannelData.iNumKeyFrames;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
		m_vecKeyFrames.push_back(ChannelData.pKeyFrames[i]);

	return S_OK;
}

CChannel* CChannel::Create(const CHANNELDATA& ChannelData)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(ChannelData)))
	{
		MSG_BOX("Failed to Create CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	m_vecKeyFrames.clear();
}
