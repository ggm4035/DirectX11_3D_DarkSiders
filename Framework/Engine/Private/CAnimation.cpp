#include "CAnimation.h"

#include "CChannel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const ANIMATIONDATA& AnimationData)
{
	strcpy_s(m_szName, AnimationData.szName);
	m_Duration = AnimationData.Duration;
	m_TickPerSec = AnimationData.TickPerSec;

	if (FAILED(Ready_Channels(AnimationData)))
		return E_FAIL;

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(const _double& TimeDelta)
{
	/* 시간 값에 따른 프레임의 위치를 파악해서 현재프레임과 다음프레임 사이를
	선형보간하는 형태로 진행한다. */

	_matrix TransformationMatrix;
}

HRESULT CAnimation::Ready_Channels(const ANIMATIONDATA& AnimationData)
{
	m_iNumChannels = AnimationData.iNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(AnimationData.pChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_vecChannels.push_back(pChannel);
	}

	return S_OK;
}

CAnimation* CAnimation::Create(const ANIMATIONDATA& AnimationData)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(AnimationData)))
	{
		MSG_BOX("Failed to Create CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
}
