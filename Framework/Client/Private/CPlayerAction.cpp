#include "stdafx.h"
#include "CPlayerAction.h"

#include "CGameInstance.h"
#include "CPlayer.h"
#include "CMoveAction.h"
#include "CJumpAction.h"
#include "CAttackAction.h"

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
			m_pModelCom->Change_Animation("Run_F");
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

		case Client::CPlayerAction::STATE_HATK_1:
			m_pModelCom->Change_Animation("Atk_Heavy_01");
			break;

		case Client::CPlayerAction::STATE_HATK_2:
			m_pModelCom->Change_Animation("Atk_Heavy_02");
			break;

		case Client::CPlayerAction::STATE_HATK_3:
			m_pModelCom->Change_Animation("Atk_Heavy_03");
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
		}

		m_ePreState = m_eCurState;
	}
}

HRESULT CPlayerAction::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	m_eCurState = STATE_IDLE;

	m_pModelCom = dynamic_cast<CModel*>(dynamic_cast<CPlayer*>(pOwner)->Get_Component(L"Com_Model"));
	Safe_AddRef(m_pModelCom);

	if (FAILED(AssembleBehaviors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerAction::Tick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Key_Pressing(DIK_W);
	pGameInstance->Key_Pressing(DIK_S);
	pGameInstance->Key_Pressing(DIK_D);
	pGameInstance->Key_Pressing(DIK_A);
	pGameInstance->Key_Down(DIK_LSHIFT);
	pGameInstance->Key_Down(DIK_SPACE);

	pGameInstance->Mouse_Down(CInput_Device::DIM_LB);
	pGameInstance->Mouse_Down(CInput_Device::DIM_RB);

	/* 각 액션들에게 메시지를 보낸다. */

	_ubyte Input = pGameInstance->Pop_Message();

	if ((true == m_pModelCom->isLoopAnimation() && DIK_END == Input) ||
		(true == m_pModelCom->isFinishedAnimation() && DIK_END == Input))
		Set_State(STATE_IDLE);

	while (DIK_END != Input)
	{
		switch (Input)
		{
		case CInput_Device::DIM_LB:
		case CInput_Device::DIM_RB:
			m_pAttackAction->Push_Move_Message(Input);
			break;

		case DIK_W:
		case DIK_S:
		case DIK_D:
		case DIK_A:
			m_pMoveAction->Push_Move_Message(Input);
			break;

		case DIK_SPACE:
			m_pJumpAction->Push_Jump_Message(Input);
			break;

		case DIK_LSHIFT:
			Set_State(STATE_DASH);
			break;
		}
		Input = pGameInstance->Pop_Message();
	}

	m_pMoveAction->Tick(TimeDelta);
	m_pAttackAction->Tick(TimeDelta);
	m_pJumpAction->Tick(TimeDelta);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayerAction::AssembleBehaviors()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CBehavior* pAction = dynamic_cast<CMoveAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"MoveAction", m_pOwner));
	if (nullptr == pAction)
		return E_FAIL;
	Assemble_Behavior(L"MoveAction", pAction);
	m_pMoveAction = dynamic_cast<CMoveAction*>(pAction);

	pAction = dynamic_cast<CJumpAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"JumpAction", m_pOwner));
	if (nullptr == pAction)
		return E_FAIL;
	Assemble_Behavior(L"JumpAction", pAction);
	m_pJumpAction = dynamic_cast<CJumpAction*>(pAction);

	pAction = dynamic_cast<CAttackAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"AttackAction", m_pOwner));
	if (nullptr == pAction)
		return E_FAIL;
	Assemble_Behavior(L"AttackAction", pAction);
	m_pAttackAction = dynamic_cast<CAttackAction*>(pAction);

	Safe_Release(pGameInstance);

	return S_OK;
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
	Safe_Release(m_pModelCom);

	Safe_Release(m_pMoveAction);
	Safe_Release(m_pJumpAction);
	Safe_Release(m_pAttackAction);

	CBehavior::Free();
}
