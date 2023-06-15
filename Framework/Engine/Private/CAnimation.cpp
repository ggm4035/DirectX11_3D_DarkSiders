#include "CAnimation.h"

#include "CChannel.h"

CAnimation::CAnimation()
	: m_isLoop(true) // �ӽ�
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_Duration(rhs.m_Duration)
	, m_TickPerSec(rhs.m_TickPerSec)
	, m_TimeAcc(rhs.m_TimeAcc)
	, m_isFinished(rhs.m_isFinished)
	, m_isLoop(rhs.m_isLoop)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_vecChannels(rhs.m_vecChannels)
	, m_vecChannelCurrentKeyFrames(rhs.m_vecChannelCurrentKeyFrames)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& Channel : m_vecChannels)
		Safe_AddRef(Channel);
}

HRESULT CAnimation::Initialize(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones)
{
	strcpy_s(m_szName, AnimationData.szName);
	m_Duration = AnimationData.Duration;
	m_TickPerSec = AnimationData.TickPerSec;

	if (FAILED(Ready_Channels(AnimationData, Bones)))
		return E_FAIL;

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(CModel::BONES& Bones, const _double& TimeDelta)
{
	/* �ð� ���� ���� �������� ��ġ�� �ľ��ؼ� ���������Ӱ� ���������� ���̸�
	���������ϴ� ���·� �����Ѵ�. */

	m_TimeAcc += m_TickPerSec * TimeDelta;

	if (m_TimeAcc >= m_Duration)
	{
		if (true == m_isLoop)
			m_TimeAcc = 0.0; 
		else
			m_isFinished = true;
	}

	/* ���� ����� �ð��� �µ��� ��� ���� ���¸� Ű������������ ��������Ͽ� �����Ѵ�. */
	_uint iChannelIndex = { 0 };
	for (auto& pChannel : m_vecChannels)
	{
		if (nullptr == pChannel)
			return;

		pChannel->Invalidate_TransformationMatrix(Bones, m_TimeAcc, &m_vecChannelCurrentKeyFrames[iChannelIndex++]);
	}
}

HRESULT CAnimation::Ready_Channels(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones)
{
	m_iNumChannels = AnimationData.iNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(AnimationData.pChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_vecChannels.push_back(pChannel);
	}

	m_vecChannelCurrentKeyFrames.resize(m_iNumChannels);

	return S_OK;
}

CAnimation* CAnimation::Create(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(AnimationData, Bones)))
	{
		MSG_BOX("Failed to Create CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& Channel : m_vecChannels)
		Safe_Release(Channel);
}
