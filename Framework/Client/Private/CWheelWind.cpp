
#include "CWheelWind.h"

#include "CGameInstance.h"
#include "CPlayerAction.h"
#include "CPlayer.h"
#include "CModel.h"
#include "CAction.h"
#include "CSequence.h"
#include "CWait.h"
#include "CWeapon.h"

CWheelWind::CWheelWind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CWheelWind::CWheelWind(const CWheelWind& rhs)
	: CBehavior(rhs)
{
}

HRESULT CWheelWind::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWheelWind::Tick(const _double& TimeDelta)
{
	m_fCoolTime += TimeDelta;

	if (false == m_isPlay)
		return S_OK;

	m_fTimeAcc += TimeDelta;

	dynamic_cast<CWeapon*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Parts(L"Weapon"))->On_SwordTrail();
	if (7.5f > m_fTimeAcc && m_fTimeAcc > m_fTick)
	{
		CGameInstance::GetInstance()->Play_Sound(L"en_legionchampion_atk_whirlwind_whoosh_02.ogg", CSound_Manager::SOUND_PLAYER, 0.4f, true);
		m_fTick += 0.4f;
	}
	
	HRESULT hr = S_OK;

	for (auto& BehaviorDesc : m_BehaviorList)
		hr = BehaviorDesc.pBehavior->Tick(TimeDelta);

	if (hr == BEHAVIOR_SUCCESS)
	{
		dynamic_cast<CWeapon*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Parts(L"Weapon"))->Off_SwordTrail();
		Reset();
	}
	
	return S_OK;
}

void CWheelWind::Reset()
{
	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	pAction->Set_State(CPlayerAction::STATE_IDLE);
	dynamic_cast<CPlayer*>(m_pOwner)->Get_Collider(L"Col_WheelWind")->Set_Enable(false);
	m_isPlay = false;
	m_fTimeAcc = 0.f;
	m_fTick = 0.f;
	m_fCoolTime = 0.f;
}

void CWheelWind::Play()
{
	if (false == m_isPlay && 3.f <= m_fCoolTime)
	{
		CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);
		
		pAction->Set_State(CPlayerAction::STATE_WHEEL);

		m_isPlay = true;
		m_fTimeAcc = 0.f;
	}
}

HRESULT CWheelWind::AssembleBehaviors()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSequence* pSequence = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence", m_pOwner));
	if (nullptr == pSequence)
		return E_FAIL;

	CAction* pAction_Start = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_Start)
		return E_FAIL;
	CAction* pAction_Ing = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_Ing)
		return E_FAIL;
	CAction* pAction_End = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_End)
		return E_FAIL;

	CWait* pWait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Wait", m_pOwner));
	if (nullptr == pWait)
		return E_FAIL;

	pAction_Start->Bind_AnimationTag("Wheel_Enter");
	pAction_Ing->Bind_AnimationTag("Wheel_Stay");
	pAction_Ing->NotLerp();
	pAction_End->Bind_AnimationTag("Wheel_Exit");
	pWait->Set_Timer(7.f);

	if (FAILED(Assemble_Behavior(L"Sequence", pSequence)))
		return E_FAIL;

	if (FAILED(pSequence->Assemble_Behavior(L"Action_Start", pAction_Start)))
		return E_FAIL;
	if (FAILED(pSequence->Assemble_Behavior(L"Action_Ing", pAction_Ing)))
		return E_FAIL;
	if (FAILED(pSequence->Assemble_Behavior(L"Action_End", pAction_End)))
		return E_FAIL;

	if (FAILED(pAction_Ing->Assemble_Behavior(L"Tsk_Wait", pWait)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CWheelWind* CWheelWind::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWheelWind* pInstance = new CWheelWind(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAttackAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CWheelWind* CWheelWind::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CWheelWind* pInstance = new CWheelWind(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAttackAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWheelWind::Free()
{
	CBehavior::Free();
}
