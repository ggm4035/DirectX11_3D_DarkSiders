#include "CPattern_Attack.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CLookAtTarget.h"
#include "CAction.h"
#include "CFollow.h"
#include "CSequence.h"

CPattern_Attack::CPattern_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSelector(pDevice, pContext)
{
}

CPattern_Attack::CPattern_Attack(const CPattern_Attack& rhs)
	: CSelector(rhs)
{
}

HRESULT CPattern_Attack::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CSelector::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	CGameObject3D* pGameObject = dynamic_cast<CGameObject3D*>(m_pOwner);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_pModel = dynamic_cast<CModel*>(pGameObject->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	/* ÄðÅ¸ÀÓ */
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_float* pTimeAcc = { nullptr };
			pBlackBoard->Get_Type(L"fTimeAcc", pTimeAcc);
			if (nullptr == pTimeAcc)
				return false;

			_float fInterval = *pTimeAcc - m_fPreTimeAcc;
			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CGameObject3D::HITSTATE* pCurHitState;
			pBlackBoard->Get_Type(L"eCurHitState", pCurHitState);

			if (CGameObject3D::NONE == *pCurHitState)
				return true;

			return false;
		});

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsRangeInPlayer = { nullptr };
			pBlackBoard->Get_Type(L"isRangeInPlayer", pIsRangeInPlayer);
			if (nullptr == pIsRangeInPlayer)
				return false;

			return *pIsRangeInPlayer;
		});

	return S_OK;
}

HRESULT CPattern_Attack::Tick(const _double& TimeDelta)
{
	HRESULT hr = CSelector::Tick(TimeDelta);

	_float* pTimeAcc = { nullptr };
	m_pBlackBoard->Get_Type(L"fTimeAcc", pTimeAcc);
	if (nullptr == pTimeAcc)
		return E_FAIL;

	if (BEHAVIOR_SUCCESS == hr)
		m_fPreTimeAcc = *pTimeAcc;

	return hr;
}

HRESULT CPattern_Attack::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSequence* pSequence_Attack = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence", m_pOwner));
	if (nullptr == pSequence_Attack)
		return E_FAIL;
	CRandomSelector* pRandom_Attack = dynamic_cast<CRandomSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"RandomSelector", m_pOwner));
	if (nullptr == pRandom_Attack)
		return E_FAIL;

	/* Action */
	CAction* pAction_Follow = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_Follow)
		return E_FAIL;
	CAction* pAction_LookAt = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_LookAt)
		return E_FAIL;

	/* Movement */
	CFollow* pFollow = dynamic_cast<CFollow*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Follow", m_pOwner));
	if (nullptr == pFollow)
		return E_FAIL;
	CLookAtTarget* pLookAtTarget = dynamic_cast<CLookAtTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_LookAtTarget", m_pOwner));
	if (nullptr == pLookAtTarget)
		return E_FAIL;

	pAction_Follow->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsAbleAttack = { nullptr };

			pBlackBoard->Get_Type(L"isAbleAttack", pIsAbleAttack);
			if (nullptr == pIsAbleAttack)
				return false;

			return !(*pIsAbleAttack);
		});

	pAction_Follow->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			if (true == m_pModel->isLoopAnimation() ||
				true == m_pModel->isFinishedAnimation())
				return true;

			return false;
		});

	pAction_Follow->Bind_AnimationTag(m_strAnimationTag);
	pAction_LookAt->Bind_AnimationTag(m_strAnimationTag);

	pFollow->Bind_Move_Speed(0.9f);
	pFollow->Bind_Turn_Speed(5.f);
	pLookAtTarget->Set_Timer(1.5f);

	if (FAILED(Assemble_Behavior(L"Action_Follow", pAction_Follow)))
		return E_FAIL;
	if (FAILED(Assemble_Behavior(L"Sequence_Attack", pSequence_Attack)))
		return E_FAIL;

	if (FAILED(pAction_Follow->Assemble_Behavior(L"Tsk_Follow", pFollow)))
		return E_FAIL;
	if (FAILED(pSequence_Attack->Assemble_Behavior(L"Action_LookAt", pAction_LookAt)))
		return E_FAIL;
	if (FAILED(pSequence_Attack->Assemble_Behavior(L"Random_Attack", pRandom_Attack)))
		return E_FAIL;

	if (FAILED(pAction_LookAt->Assemble_Behavior(L"Tsk_LookAtTarget", pLookAtTarget)))
		return E_FAIL;

	for (auto& strAnimTag : m_AttackAnimTagList)
	{
		CAction* pAction_Attack = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
		if (nullptr == pAction_Attack)
			return E_FAIL;

		pAction_Attack->Bind_AnimationTag(strAnimTag);
		if (FAILED(pRandom_Attack->Assemble_Behavior(pGameInstance->strToWStr(strAnimTag), pAction_Attack)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CPattern_Attack* CPattern_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPattern_Attack* pInstance = new CPattern_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPattern_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPattern_Attack* CPattern_Attack::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPattern_Attack* pInstance = new CPattern_Attack(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CPattern_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPattern_Attack::Free()
{
	if(true == m_isCloned)
		Safe_Release(m_pModel);

	CSelector::Free();
}
