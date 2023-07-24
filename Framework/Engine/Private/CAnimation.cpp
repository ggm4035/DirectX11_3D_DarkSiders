#include "CAnimation.h"

#include "CGameInstance.h"
#include "CGameObject3D.h"
#include "CChannel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_isLoop(rhs.m_isLoop)
	, m_strName(rhs.m_strName)
	, m_Duration(rhs.m_Duration)
	, m_TickPerSec(rhs.m_TickPerSec)
	, m_vecNotifys(rhs.m_vecNotifys)
	, m_vecChannels(rhs.m_vecChannels)
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

	if (FAILED(Ready_Channels(AnimationData, Bones)))
		return E_FAIL;

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(CModel::BONES& Bones, const _double& TimeDelta)
{
	/* �ð� ���� ���� �������� ��ġ�� �ľ��ؼ� ���������Ӱ� ���������� ���̸�
	���������ϴ� ���·� �����Ѵ�. */

#if defined(_USE_IMGUI) || defined(_DEBUG)
	if (true == m_isPause)
	{
		_uint channelIndex = { 0 };
		for (auto& pChannel : m_vecChannels)
		{
			if (nullptr == pChannel)
				return;

			pChannel->Invalidate_TransformationMatrix(Bones, m_TimeAcc, &m_vecChannelCurrentKeyFrames[channelIndex++]);
		}
		return;
	}
#endif

	/* Lerp�� �������� ��쿡�� �ִϸ��̼��� �������� �ʴ´�. */
	/* ������ �ʱ⿡ ��ġ�� ��������ߵ�*/
	if (false == m_isLerped)
	{
		/* ������ ������ �ٷ� �ִϸ��̼� �����ϰ� �Ϸ��� */
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

	/* ���� ����� �ð��� �µ��� ��� ���� ���¸� Ű������������ ��������Ͽ� �����Ѵ�. */
	_uint iChannelIndex = { 0 };
	for (auto& pChannel : m_vecChannels)
	{
		if (nullptr == pChannel)
			return;

		pChannel->Invalidate_TransformationMatrix(Bones, m_TimeAcc, &m_vecChannelCurrentKeyFrames[iChannelIndex++]);
	}

	for (auto& NotifyDesc : m_vecNotifys)
	{
		if (false == NotifyDesc.isNotify)
		{
			if (m_TimeAcc >= NotifyDesc.fAnimTime)
			{
				Notify(NotifyDesc.fAnimTime);
			}
		}
	}
}

void CAnimation::Reset_Animation()
{
	m_isLerped = true;
	m_isFinished = false;

	m_TimeAcc = 0.0;
	for (auto& iIndex : m_vecChannelCurrentKeyFrames)
		iIndex = 0;

	Reset_Notifys();
}

void CAnimation::Reset_Notifys()
{
	for (auto& Notify : m_vecNotifys)
	{
		Notify.isNotify = false;
		for (auto ObserverDesc : Notify.vecObserver)
		{
			switch (ObserverDesc.eType)
			{
			case ANIMATION:
				reinterpret_cast<CAnimation*>(ObserverDesc.pObserver)->m_isAbleChange = false;
				break;

			case COLLIDER:
				reinterpret_cast<CCollider*>(ObserverDesc.pObserver);
				break;
			}
		}
	}
}

HRESULT CAnimation::Bind_Notifys(class CGameObject3D* pGameObject, vector<NOTIFYDATA>& vecNotifyData)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	wstring wstrName = pGameInstance->strToWStr(m_strName);

	for (auto& NotifyData : vecNotifyData)
	{
		NOTIFYDESC NotifyDesc;
		
		NotifyDesc.fAnimTime = NotifyData.fPoint;

		for (auto& ObserverData : NotifyData.vecObservers)
		{
			OBSERVERDESC ObserverDesc;

			switch (ObserverData.iObserverType)
			{
			case ANIMATION:
				ObserverDesc.pObserver = this;
				ObserverDesc.pParam = nullptr;
				ObserverDesc.wstrObserverTag = wstrName;
				ObserverDesc.eType = ANIMATION;
				break;

			case COLLIDER:
				CCollider* pCollider = dynamic_cast<CCollider*>(pGameObject->Find_Collider(ObserverData.wstrNotifyTag));
				if (nullptr == pCollider)
				{
					pCollider = dynamic_cast<CCollider*>(pGameObject->Find_Collider_For_Parts(ObserverData.wstrNotifyTag));
					if (nullptr == pCollider)
						continue;
				}
				ObserverDesc.pObserver = reinterpret_cast<IObserver_Animation*>(pCollider);

				CCollider::OBVCOLPARAMS ObserverParams;
				ObserverParams.isEnable = ObserverData.isEnable;
				ObserverDesc.pParam = &ObserverParams;
				ObserverDesc.wstrObserverTag = wstrName;
				ObserverDesc.eType = COLLIDER;
				break;
			}
			NotifyDesc.vecObserver.push_back(ObserverDesc);
		}
		m_vecNotifys.push_back(NotifyDesc);
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CAnimation::Notify(const float& fPoint)
{
	auto pObserverList = Find_ObserverList(fPoint);

	if (nullptr != pObserverList)
	{
		for (auto& ObserverDesc : *pObserverList)
		{
			switch (ObserverDesc.eType)
			{
			case ANIMATION:
				reinterpret_cast<CAnimation*>(ObserverDesc.pObserver)->Update_Observer(ObserverDesc.pParam);
				break;

			case COLLIDER:
				reinterpret_cast<CCollider*>(ObserverDesc.pObserver)->Update_Observer(ObserverDesc.pParam);
				break;
			}
		}
	}
}

HRESULT CAnimation::Ready_Channels(const ANIMATIONDATA& AnimationData, const CModel::BONES& Bones)
{
	m_iNumChannels = AnimationData.iNumChannels;

	string strRootTag = "";

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(AnimationData.vecChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		strRootTag = AnimationData.vecChannels[i].szName;
		if (string::npos != strRootTag.find("Bone") &&
			string::npos != strRootTag.find("Root"))
		{
#if defined(_USE_IMGUI) || defined(_DEBUG)
			m_iMaxNumRootFrames = AnimationData.vecChannels[i].iNumKeyFrames;
#endif
			m_iRootBoneIndex = i;
		}

		m_vecChannels.push_back(pChannel);

#if defined(_USE_IMGUI) || defined(_DEBUG)

		if (m_iMaxNumFrames < AnimationData.vecChannels[i].iNumKeyFrames)
		{
			m_iMaxNumFrames = AnimationData.vecChannels[i].iNumKeyFrames;
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

	/* Lerp ���� ( ���޹��� ���� �ִϸ��̼��� ���� ��ġ���� ���� �ִϸ��̼��� ���ۺκ� ) */
	_uint iChannelIndex = { 0 };
	for (auto& pChannel : m_pLerpAnimation->m_vecChannels)
	{
		pChannel->Lerp_TransformationMatrix(Bones, m_LerpDuration, m_TimeAcc,
			m_pLerpAnimation->m_vecChannelCurrentKeyFrames[iChannelIndex],
			m_vecChannels[iChannelIndex]->Get_FirstKeyFrame());
		++iChannelIndex;
	}

	/* Lerp�� ������� */
	if (m_TimeAcc >= m_LerpDuration)
	{
		Reset_Animation();

		m_pLerpAnimation->Reset_Animation();
		return true;
	}

	return false;
}

vector<CAnimation::OBSERVERDESC>* CAnimation::Find_ObserverList(const _float& fPoint)
{
	auto iter = find_if(m_vecNotifys.begin(), m_vecNotifys.end(), [&](const NOTIFYDESC& NotifyDesc)
		{
			if (NotifyDesc.fAnimTime == fPoint)
				return true;
			else
				return false;
		});

	if (iter == m_vecNotifys.end())
		return nullptr;

	return &iter->vecObserver;
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