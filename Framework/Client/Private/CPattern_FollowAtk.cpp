#include "CPattern_FollowAtk.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CLookAtTarget.h"
#include "CFollow.h"
#include "CSequence.h"

CPattern_FollowAtk::CPattern_FollowAtk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSelector(pDevice, pContext)
{
}

CPattern_FollowAtk::CPattern_FollowAtk(const CPattern_FollowAtk& rhs)
	: CSelector(rhs)
{
}

HRESULT CPattern_FollowAtk::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CHealth* pHealth = { nullptr };
			pBlackBoard->Get_Type(L"pHealth", pHealth);
			if (nullptr == pHealth)
				return false;

			if (CHealth::HIT_NONE != pHealth->Get_Current_HitState())
				return false;

			return true;
		});

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsSpawn = { nullptr };
			pBlackBoard->Get_Type(L"isSpawn", pIsSpawn);
			if (nullptr == pIsSpawn)
				return false;

			return *pIsSpawn;
		});

	return S_OK;
}

HRESULT CPattern_FollowAtk::Tick(const _double& TimeDelta)
{
	return CSelector::Tick(TimeDelta);
}

HRESULT CPattern_FollowAtk::Assemble_Childs()
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
			if (true == m_pModel->isLoopAnimation() ||
				true == m_pModel->isFinishedAnimation())
				return true;

			return false;
		});

	pAction_Follow->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsAbleAttack = { nullptr };

			pBlackBoard->Get_Type(L"isAbleAttack", pIsAbleAttack);
			if (nullptr == pIsAbleAttack)
				return false;

			return !(*pIsAbleAttack);
		});

	pAction_Follow->Bind_AnimationTag("Run");
	pAction_LookAt->Bind_AnimationTag("Run");

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

	for (auto& Desc : m_Attacks)
	{
		CAction* pAction_Attack = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
		if (nullptr == pAction_Attack)
			return E_FAIL;

		pAction_Attack->Bind_AnimationTag(Desc.strAttackAnimTag);
		if (FAILED(pRandom_Attack->Assemble_Behavior(pGameInstance->strToWStr(Desc.strAttackAnimTag), pAction_Attack)))
			return E_FAIL;

		for (auto& Sound : Desc.Sounds)
			pAction_Attack->Add_Sound(Sound);
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CPattern_FollowAtk* CPattern_FollowAtk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPattern_FollowAtk* pInstance = new CPattern_FollowAtk(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPattern_FollowAtk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPattern_FollowAtk* CPattern_FollowAtk::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPattern_FollowAtk* pInstance = new CPattern_FollowAtk(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CPattern_FollowAtk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPattern_FollowAtk::Free()
{
	if (true == m_isCloned)
		Safe_Release(m_pModel);

	CSelector::Free();
}
