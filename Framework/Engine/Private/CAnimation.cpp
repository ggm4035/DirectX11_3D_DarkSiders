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
	/* �ð� ���� ���� �������� ��ġ�� �ľ��ؼ� ���������Ӱ� ���������� ���̸�
	���������ϴ� ���·� �����Ѵ�. */

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
