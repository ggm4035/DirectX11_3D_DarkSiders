#include "CAnimation.h"

#include "CChannel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_isLoop(rhs.m_isLoop)
	, m_strName(rhs.m_strName)
	, m_Duration(rhs.m_Duration)
	, m_TickPerSec(rhs.m_TickPerSec)
	, m_vecChannels(rhs.m_vecChannels)
	, m_vecTimeRange(rhs.m_vecTimeRange)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_iRootBoneIndex(rhs.m_iRootBoneIndex)
	, m_isFollowAnimation(rhs.m_isFollowAnimation)
	, m_vecChannelCurrentKeyFrames(rhs.m_vecChannelCurrentKeyFrames)
#if defined(_USE_IMGUI) || defined(_DEBUG)
	, m_iMaxNumFrames(rhs.m_iMaxNumFrames)
	, m_iMaxFramesIndex(rhs.m_iMaxFramesIndex)
	, m_iMaxNumRootFrames(rhs.m_iMaxNumRootFrames)
#endif
{
	for (auto& Channel : m_vecChannels)
		Safe_AddRef(Channel);
}

void CAnimation::Bind_LerpAnimation(CAnimation* pLerpAnimation)
{
	m_TimeAcc = 0.0;
	m_pLerpAnimation = pLerpAnimation;
	m_isFinished = false;
	m_isLerped = false;
}

HRESULT CAnimation::Initialize(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones)
{
	m_isLoop = AnimationData.bIsLoop;
	m_strName = AnimationData.szName;
	m_Duration = AnimationData.Duration;
	m_TickPerSec = AnimationData.TickPerSec;
	m_isFollowAnimation = AnimationData.bIsFollowAnimation;

	if (FAILED(Ready_TimeRanges(AnimationData)))
		return E_FAIL;

	if (FAILED(Ready_Channels(AnimationData, Bones)))
		return E_FAIL;

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(CModel::BONES& Bones, const _double& TimeDelta)
{
	m_isAbleChange = false;
	/* 시간 값에 따른 프레임의 위치를 파악해서 현재프레임과 다음프레임 사이를
	선형보간하는 형태로 진행한다. */
	if (true == m_isPause)
		return;

	/* Lerp가 진행중일 경우에는 애니메이션을 진행하지 않는다. */
	/* 하지만 초기에 위치를 설정해줘야됨*/
	if (false == m_isLerped)
	{
		/* 러프가 끝나면 바로 애니메이션 진행하게 하려고 */
		if (false == Lerp_Animation(Bones, TimeDelta))
			return;
	}

	m_isBlock = false;

	m_TimeAcc += m_TickPerSec * TimeDelta;

	if (m_TimeAcc >= m_Duration)
	{
		if (true == m_isLoop)
			m_TimeAcc = 0.0;
		else
			m_isFinished = true;
	}

	/* 현재 재생된 시간에 맞도록 모든 뼈의 상태를 키프레임정보를 기반으로하여 갱신한다. */
	_uint iChannelIndex = { 0 };
	for (auto& pChannel : m_vecChannels)
	{
		if (nullptr == pChannel)
			return;

		pChannel->Invalidate_TransformationMatrix(Bones, m_TimeAcc, &m_vecChannelCurrentKeyFrames[iChannelIndex++]);
	}
}

void CAnimation::Reset_Animation()
{
	m_isLerped = true;
	m_isFinished = false;

	m_TimeAcc = 0.0;
	for (auto& iIndex : m_vecChannelCurrentKeyFrames)
		iIndex = 0;
}

HRESULT CAnimation::Ready_TimeRanges(const ANIMATIONDATA& AnimationData)
{
	for (_uint i = 0; i < AnimationData.iNumRanges; ++i)
	{
		TIMERANGE TimeRange;
		TimeRange.fStartPoint = AnimationData.pTimeRanges[i].fStartPoint;
		TimeRange.fEndPoint = AnimationData.pTimeRanges[i].fEndPoint;

		for (_uint j = 0; j < OBSERVERTYPE::TYPE_END; ++j)
		{
			TimeRange.arrTypes[j] = OBSERVERTYPE::TYPE_END;

			if (OBSERVERTYPE(j) == AnimationData.pTimeRanges[i].arrTypes[j])
				TimeRange.arrTypes[j] = OBSERVERTYPE(j);
		}

		m_vecTimeRange.push_back(TimeRange);
	}

	return S_OK;
}

HRESULT CAnimation::Ready_Channels(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones)
{
	m_iNumChannels = AnimationData.iNumChannels;

	string strRootTag = "";

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(AnimationData.pChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		strRootTag = AnimationData.pChannels[i].szName;
		if (string::npos != strRootTag.find("Bone") &&
			string::npos != strRootTag.find("Root"))
		{
#if defined(_USE_IMGUI) || defined(_DEBUG)
			m_iMaxNumRootFrames = AnimationData.pChannels[i].iNumKeyFrames;
#endif
			m_iRootBoneIndex = i;
		}

		m_vecChannels.push_back(pChannel);

#if defined(_USE_IMGUI) || defined(_DEBUG)

		if (m_iMaxNumFrames < AnimationData.pChannels[i].iNumKeyFrames)
		{
			m_iMaxNumFrames = AnimationData.pChannels[i].iNumKeyFrames;
			m_iMaxFramesIndex = i;
		}

#endif
	}

	m_vecChannelCurrentKeyFrames.resize(m_iNumChannels);

	return S_OK;
}

_bool CAnimation::Lerp_Animation(CModel::BONES& Bones, const _double& TimeDelta)
{
	m_TimeAcc += TimeDelta;

	/* Lerp 구간 ( 전달받은 이전 애니메이션의 현재 위치부터 현재 애니메이션의 시작부분 ) */
	_uint iChannelIndex = { 0 };
	for (auto& pChannel : m_pLerpAnimation->m_vecChannels)
	{
		pChannel->Lerp_TransformationMatrix(Bones, m_LerpDuration, m_TimeAcc,
			m_pLerpAnimation->m_vecChannelCurrentKeyFrames[iChannelIndex],
			m_vecChannels[iChannelIndex]->Get_FirstKeyFrame());
		++iChannelIndex;
	}

	/* Lerp가 끝난경우 */
	if (m_TimeAcc >= m_LerpDuration)
	{
		Reset_Animation();

		m_pLerpAnimation->Reset_Animation();
		return true;
	}

	return false;
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

#if defined(_USE_IMGUI) || defined(_DEBUG)

vector<KEYFRAME>& CAnimation::Get_KeyFrames()
{
	return m_vecChannels[m_iMaxFramesIndex]->Get_KeyFrames();
}

vector<KEYFRAME>& CAnimation::Get_RootKeyFrames()
{
	return m_vecChannels[m_iRootBoneIndex]->Get_KeyFrames();
}

void CAnimation::Set_KeyFrame(const _uint& iIndex)
{
	for (_uint i = 0; i < m_vecChannels.size(); ++i)
	{
		m_vecChannelCurrentKeyFrames[i] = iIndex >= m_vecChannels[i]->Get_KeyFrames().size() - 1 ?
			m_vecChannels[i]->Get_KeyFrames().size() - 2 : iIndex;
	}

	m_TimeAcc = m_vecChannels[m_iMaxFramesIndex]->Get_KeyFrames()[1 > iIndex ? 0 : iIndex - 1].Time;
}

void CAnimation::Set_RootKeyFrame(const _uint& iIndex)
{
	for (_uint i = 0; i < m_vecChannels.size(); ++i)
	{
		m_vecChannelCurrentKeyFrames[i] = iIndex >= m_vecChannels[i]->Get_KeyFrames().size() - 1 ?
			m_vecChannels[i]->Get_KeyFrames().size() - 2 : iIndex;
	}

	m_TimeAcc = m_vecChannels[m_iRootBoneIndex]->Get_KeyFrames()[1 > iIndex ? 0 : iIndex - 1].Time;
}

#endif