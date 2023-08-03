
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

	if (true == m_isFirst)
	{
		CGameInstance::GetInstance()->Play_Sound(m_wstrSoundTag.c_str(), CSound_Manager::SOUND_ENEMY, 0.5f);
		m_pModel->Change_Animation(m_strAnimationTag, m_isLerp);
		m_isFirst = false;
	}

	/* 행동 체크 */
	for (auto& BehaviorDesc : m_BehaviorList)
	{
		BehaviorDesc.result = BehaviorDesc.pBehavior->Tick(TimeDelta);
		if (BEHAVIOR_RUNNING == BehaviorDesc.result)
			m_isFinishBehaviors = false;
	}

	if (true == m_isFinishBehaviors ||
		true == m_pModel->isAbleChangeAnimation() ||
		true == m_pModel->isFinishedAnimation())
	{
		if (true == m_isOneTimeAction)
			m_isFirstAction = false;

		m_isFirst = true;
		m_isFinishBehaviors = false;

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
