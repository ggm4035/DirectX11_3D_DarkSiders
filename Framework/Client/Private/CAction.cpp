
#include "CAction.h"

#include "CGameInstance.h"
#include "CBlackBoard.h"
#include "CGameObject3D.h"
#include "CModel.h"

CAction::CAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CAction::CAction(const CAction& rhs)
	: CBehavior(rhs)
{
}

HRESULT CAction::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	CGameObject3D* pGameObject = dynamic_cast<CGameObject3D*>(m_pOwner);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_pModel = dynamic_cast<CModel*>(pGameObject->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	/* 쿨타임 */
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_float* pTimeAcc = { nullptr };
			pBlackBoard->Get_Type(L"fTimeAcc", pTimeAcc);
			if (nullptr == pTimeAcc)
			{
				MSG_BOX("is Not fTimeAcc");
				return false;
			}

			_float fInterval = *pTimeAcc - m_fPreTimeAcc;
			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			if (true == m_isOneTimeAction &&
				false == m_isFirstAction)
				return false;

			return true;
		});

	return S_OK;
}

HRESULT CAction::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
	{
		m_isFirst = true;
		m_isFinishBehaviors = false;
		return BEHAVIOR_FAIL;
	}

	/* 행동이 하나라도 있으면 행동체크 활성화 */
	if (0 != m_BehaviorList.size())
		m_isFinishBehaviors = true;

	PlaySounds(TimeDelta);

	if (true == m_isFirst)
	{
		if (0 < m_wstrBgmTag.size())
		{
			CGameInstance::GetInstance()->Play_BGM(m_wstrBgmTag.c_str(), 0.5f,
				CSound_Manager::SOUND_SUB_BGM);
		}
		m_pModel->Change_Animation(m_strAnimationTag, m_isLerp);
		m_isFirst = false;
	}

	/* 행동 체크 */
	if (false == m_isStopBehavior)
	{
		for (auto& BehaviorDesc : m_BehaviorList)
		{
			BehaviorDesc.result = BehaviorDesc.pBehavior->Tick(TimeDelta);
			if (BEHAVIOR_RUNNING == BehaviorDesc.result)
				m_isFinishBehaviors = false;
		}
	}
	if (true == m_isFinishBehaviors)
	{
		m_isStopBehavior = true;
	}

	_bool bCheck = { false };

	if (false == m_pModel->isLoopAnimation())
	{
		if (true == m_pModel->isAbleChangeAnimation() ||
			true == m_pModel->isFinishedAnimation())
			bCheck = true;
	}
	
	else if(true == m_isFinishBehaviors)
	{
		bCheck = true;
	}

	if (true == bCheck)
	{
		if (true == m_isOneTimeAction)
			m_isFirstAction = false;

		m_isFirst = true;
		m_isFinishBehaviors = false;
		m_isStopBehavior = false;

		_float* pTimeAcc = { nullptr };
		m_pBlackBoard->Get_Type(L"fTimeAcc", pTimeAcc);
		if (nullptr == pTimeAcc)
			return E_FAIL;

		m_fTimeAcc = 0.f;
		m_fPreTimeAcc = *pTimeAcc;

		if(0 < m_wstrBgmTag.size())
			CGameInstance::GetInstance()->Stop_Sound(CSound_Manager::SOUND_SUB_BGM);
		for (auto& Sound : m_Sounds)
			Sound.isPlaySound = false;

		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

CAction* CAction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAction* pInstance = new CAction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAction::PlaySounds(const _double& TimeDelta)
{
	m_fTimeAcc += TimeDelta;

	for (auto& Desc : m_Sounds)
	{
		if (true == Desc.isPlaySound)
			continue;

		if (Desc.fTime > m_fTimeAcc)
			continue;

		if (FAILED(CGameInstance::GetInstance()->Play_Sound(Desc.wstrSoundTag.c_str(), Desc.eChennel, Desc.fVolum, true)))
			return;
		Desc.isPlaySound = true;
	}
}

CAction* CAction::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CAction* pInstance = new CAction(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAction::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
