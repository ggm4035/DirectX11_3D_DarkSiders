
#include "CPlayerAction.h"

#include "CGameInstance.h"
#include "CBlackBoard.h"
#include "CPlayer.h"
#include "CPlayerHit.h"
#include "CPlayerMove.h"
#include "CPlayerJump.h"
#include "CPlayerAttack.h"
#include "CPlayerKnockback.h"

#include "CWheelWind.h"
#include "CLeapAttack.h"

CPlayerAction::CPlayerAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CPlayerAction::CPlayerAction(const CPlayerAction& rhs)
	: CBehavior(rhs)
{
}

void CPlayerAction::Set_State(STATE eState)
{
	m_eCurState = eState;

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CPlayerAction::STATE_IDLE:
			m_pModelCom->Change_Animation("Idle");
			break;

		case Client::CPlayerAction::STATE_RUN:
			m_pModelCom->Change_Animation("Run");
			break;

		case Client::CPlayerAction::STATE_LATK_1:
			m_pModelCom->Change_Animation("Atk_Light_01");
			break;

		case Client::CPlayerAction::STATE_LATK_2:
			m_pModelCom->Change_Animation("Atk_Light_02");
			break;

		case Client::CPlayerAction::STATE_LATK_3:
			m_pModelCom->Change_Animation("Atk_Light_03");
			break;

		case Client::CPlayerAction::STATE_LATK_4:
			m_pModelCom->Change_Animation("Atk_Light_04");
			break;

		case Client::CPlayerAction::STATE_DASH:
			m_pModelCom->Change_Animation("Dash");
			break;

		case Client::CPlayerAction::STATE_JUMP:
			m_pModelCom->Change_Animation("Jump_Single");
			break;

		case Client::CPlayerAction::STATE_DOUBLE_JUMP:
			m_pModelCom->Change_Animation("Jump_Double");
			break;

		case Client::CPlayerAction::STATE_JUMP_LAND:
			m_pModelCom->Change_Animation("Jump_Land");
			break;

		case Client::CPlayerAction::STATE_HIT:
			m_pModelCom->Change_Animation("Impact");
			break;

		case Client::CPlayerAction::STATE_KNOCKBACK:

			if (CPlayerAction::STATE_WHEEL == m_ePreState)
			{
				Reset_Wheel();
			}
			else if (CPlayerAction::STATE_LEAP == m_ePreState)
			{
				Reset_Leap();
			}
			m_eCurState = STATE_KNOCKBACK;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

const _float* CPlayerAction::Get_LeapCoolTimePtr() const
{
	return m_pSkillLeapAttack->Get_CoolTimePtr();
}

const _float* CPlayerAction::Get_WheelCoolTimePtr() const
{
	return m_pSkillWheelWind->Get_CoolTimePtr();
}

HRESULT CPlayerAction::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	m_eCurState = STATE_IDLE;

	m_pPlayer = dynamic_cast<CPlayer*>(pOwner);
	m_pModelCom = dynamic_cast<CModel*>(m_pPlayer->Get_Component(L"Com_Model"));
	Safe_AddRef(m_pModelCom);

	return S_OK;
}

HRESULT CPlayerAction::Tick(const _double& TimeDelta)
{
	if (STATE_KNOCKBACK == m_eCurState &&
		false == m_isSuperArmor)
	{
		m_pKnockbackAction->Tick(TimeDelta);
		return S_OK;
	}

	CPlayer::HITSTATE eHitState = m_pPlayer->Get_CurHitState();
	if (eHitState != CPlayer::NONE &&
		false == m_isSuperArmor)
	{
		m_pHitAction->Tick(TimeDelta);
		return S_OK;
	}

	if (true == m_isSuperArmor)
	{
		m_pPlayer->Set_CurHitState(CPlayer::NONE);

		m_fTimeAcc += TimeDelta;

		if (m_fSuperArmor <= m_fTimeAcc)
		{
			m_isSuperArmor = false;
			m_fTimeAcc = 0.f;
		}
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Key_Pressing(DIK_W);
	pGameInstance->Key_Pressing(DIK_S);
	pGameInstance->Key_Pressing(DIK_D);
	pGameInstance->Key_Pressing(DIK_A);
	pGameInstance->Key_Down(DIK_LSHIFT);
	pGameInstance->Key_Down(DIK_SPACE);

	pGameInstance->Mouse_Down(CInput_Device::DIM_LB);

	if (pGameInstance->Key_Down(DIK_Q))
		m_pSkillWheelWind->Play();
	if (pGameInstance->Key_Down(DIK_E))
		m_pSkillLeapAttack->Play();

	/* 각 액션들에게 메시지를 보낸다. */

	_ubyte Input = pGameInstance->Pop_Message();

	if (STATE_WHEEL != m_eCurState && STATE_KNOCKBACK != m_eCurState &&
		((true == m_pModelCom->isLoopAnimation() && DIK_END == Input) ||
		(true == m_pModelCom->isFinishedAnimation() && DIK_END == Input)))
		Set_State(STATE_IDLE);

	while (DIK_END != Input)
	{
		switch (Input)
		{
		case CInput_Device::DIM_LB:
			if(STATE_WHEEL != m_eCurState)
				m_pAttackAction->Push_Move_Message(Input);
			break;
			
		case DIK_W:
		case DIK_S:
		case DIK_D:
		case DIK_A:
			m_pMoveAction->Push_Move_Message(Input);
			break;

		case DIK_SPACE:
			if (STATE_WHEEL != m_eCurState)
				m_pJumpAction->Push_Jump_Message(Input);
			break;

		case DIK_LSHIFT:
			if (STATE_KNOCKBACK != m_eCurState && STATE_DASH != m_eCurState &&
				STATE_WHEEL != m_eCurState && STATE_LEAP != m_eCurState)
			{
				Set_State(STATE_DASH);
			}
			break;
		}
		Input = pGameInstance->Pop_Message();
	}

	for (auto& pBehavior : m_BehaviorList)
		pBehavior.pBehavior->Tick(TimeDelta);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayerAction::AssembleBehaviors()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CBehavior* pAction = dynamic_cast<CPlayerHit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"PlayerHit", m_pOwner));
	if (nullptr == pAction)
		return E_FAIL;
	Assemble_Behavior(L"PlayerHit", pAction);
	m_pHitAction = dynamic_cast<CPlayerHit*>(pAction);
	
	pAction = dynamic_cast<CPlayerMove*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"PlayerMove", m_pOwner));
	if (nullptr == pAction)
		return E_FAIL;
	Assemble_Behavior(L"PlayerMove", pAction);
	m_pMoveAction = dynamic_cast<CPlayerMove*>(pAction);

	pAction = dynamic_cast<CPlayerJump*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"PlayerJump", m_pOwner));
	if (nullptr == pAction)
		return E_FAIL;
	Assemble_Behavior(L"PlayerJump", pAction);
	m_pJumpAction = dynamic_cast<CPlayerJump*>(pAction);

	pAction = dynamic_cast<CPlayerAttack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"PlayerAttack", m_pOwner));
	if (nullptr == pAction)
		return E_FAIL;
	Assemble_Behavior(L"PlayerAttack", pAction);
	m_pAttackAction = dynamic_cast<CPlayerAttack*>(pAction);

	pAction = dynamic_cast<CPlayerKnockback*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"PlayerKnockback", m_pOwner));
	if (nullptr == pAction)
		return E_FAIL;
	Assemble_Behavior(L"PlayerKnockback", pAction);
	m_pKnockbackAction = dynamic_cast<CPlayerKnockback*>(pAction);

	if (FAILED(m_pKnockbackAction->AssembleBehaviors()))
		return E_FAIL;

	pAction = dynamic_cast<CWheelWind*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"PlayerWheelWind", m_pOwner));
	if (nullptr == pAction)
		return E_FAIL;
	Assemble_Behavior(L"PlayerWheelWind", pAction);
	m_pSkillWheelWind = dynamic_cast<CWheelWind*>(pAction);

	if (FAILED(m_pSkillWheelWind->AssembleBehaviors()))
		return E_FAIL;

	pAction = dynamic_cast<CLeapAttack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"PlayerLeapAttack", m_pOwner));
	if (nullptr == pAction)
		return E_FAIL;
	Assemble_Behavior(L"PlayerLeapAttack", pAction);
	m_pSkillLeapAttack = dynamic_cast<CLeapAttack*>(pAction);
	
	Safe_Release(pGameInstance);

	return S_OK;
}

void CPlayerAction::Reset_Wheel()
{
	m_pSkillWheelWind->Reset();
}

void CPlayerAction::Reset_Leap()
{
	m_pSkillLeapAttack->Reset();
}

void CPlayerAction::Reset_Jump()
{
	m_pJumpAction->Reset();
}

CPlayerAction* CPlayerAction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerAction* pInstance = new CPlayerAction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayerAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CPlayerAction* CPlayerAction::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPlayerAction* pInstance = new CPlayerAction(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayerAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerAction::Free()
{
	Safe_Release(m_pPlayer);

	if(true == m_isCloned)
		Safe_Release(m_pModelCom);

	Safe_Release(m_pHitAction);
	Safe_Release(m_pMoveAction);
	Safe_Release(m_pJumpAction);
	Safe_Release(m_pAttackAction);

	CBehavior::Free();
}
