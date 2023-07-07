#include "stdafx.h"
#include "CAttackAction.h"

#include "CGameInstance.h"
#include "CPlayerAction.h"
#include "CPlayer.h"
#include "CModel.h"

CAttackAction::CAttackAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CAttackAction::CAttackAction(const CAttackAction& rhs)
	: CBehavior(rhs)
{
}

HRESULT CAttackAction::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	m_pModel = dynamic_cast<CModel*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	return S_OK;
}

HRESULT CAttackAction::Tick(const _double& TimeDelta)
{
	while (true != m_Qmessage.empty())
	{
		if (m_iCombo != m_iPreCombo)
		{
			m_Qmessage.pop();
			continue;
		}

		switch (m_Qmessage.front())
		{
		case CInput_Device::DIM_LB:
			m_eCurrentAttack = TYPE_LIGHT;

			if (true == m_pModel->isLoopAnimation() ||
				true == m_pModel->isAbleChangeAnimation())
				m_iCombo = (3 < ++m_iCombo) ? -1 : m_iCombo;

			break;

		case CInput_Device::DIM_RB:
			m_eCurrentAttack = TYPE_HEAVY;

			if (true == m_pModel->isLoopAnimation() ||
				true == m_pModel->isAbleChangeAnimation())
				m_iCombo = (2 < ++m_iCombo) ? -1 : m_iCombo;

			break;
		}
		m_Qmessage.pop();
	}

	if (m_iCombo == m_iPreCombo)
		return S_OK;

	switch (m_eCurrentAttack)
	{
	case Client::CAttackAction::TYPE_LIGHT:
		LightAttackCombo();
		break;

	case Client::CAttackAction::TYPE_HEAVY:
		HeavyAttackCombo();
		break;
	}

	m_iPreCombo = m_iCombo;

	return S_OK;
}

void CAttackAction::LightAttackCombo()
{
	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	switch (pAction->Get_State())
	{
	case Client::CPlayerAction::STATE_IDLE:
	case Client::CPlayerAction::STATE_RUN:
	case Client::CPlayerAction::STATE_DASH:
	case Client::CPlayerAction::STATE_JUMP:
	case Client::CPlayerAction::STATE_DOUBLE_JUMP:
		pAction->Set_State(CPlayerAction::STATE_LATK_1);
		break;

	case Client::CPlayerAction::STATE_LATK_1:
		pAction->Set_State(CPlayerAction::STATE_LATK_2);
		break;
	case Client::CPlayerAction::STATE_LATK_2:
		pAction->Set_State(CPlayerAction::STATE_LATK_3);
		break;
	case Client::CPlayerAction::STATE_LATK_3:
		pAction->Set_State(CPlayerAction::STATE_LATK_4);
		break;
	case Client::CPlayerAction::STATE_LATK_4:
		pAction->Set_State(CPlayerAction::STATE_LATK_1);
		break;
	case Client::CPlayerAction::STATE_HATK_1:
		pAction->Set_State(CPlayerAction::STATE_LATK_1);
		break;
	case Client::CPlayerAction::STATE_HATK_2:
		pAction->Set_State(CPlayerAction::STATE_LATK_1);
		break;
	case Client::CPlayerAction::STATE_HATK_3:
		pAction->Set_State(CPlayerAction::STATE_LATK_1);
		break;
	}
}

void CAttackAction::HeavyAttackCombo()
{
	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	switch (pAction->Get_State())
	{
	case Client::CPlayerAction::STATE_IDLE:
	case Client::CPlayerAction::STATE_RUN:
	case Client::CPlayerAction::STATE_DASH:
	case Client::CPlayerAction::STATE_JUMP:
	case Client::CPlayerAction::STATE_DOUBLE_JUMP:
		pAction->Set_State(CPlayerAction::STATE_HATK_1);
		break;

	case Client::CPlayerAction::STATE_LATK_1:
		pAction->Set_State(CPlayerAction::STATE_HATK_1);
		break;
	case Client::CPlayerAction::STATE_LATK_2:
		pAction->Set_State(CPlayerAction::STATE_HATK_1);
		break;
	case Client::CPlayerAction::STATE_LATK_3:
		pAction->Set_State(CPlayerAction::STATE_HATK_1);
		break;
	case Client::CPlayerAction::STATE_LATK_4:
		pAction->Set_State(CPlayerAction::STATE_HATK_1);
		break;
	case Client::CPlayerAction::STATE_HATK_1:
		pAction->Set_State(CPlayerAction::STATE_HATK_2);
		break;
	case Client::CPlayerAction::STATE_HATK_2:
		pAction->Set_State(CPlayerAction::STATE_HATK_3);
		break;
	case Client::CPlayerAction::STATE_HATK_3:
		pAction->Set_State(CPlayerAction::STATE_HATK_1);
		break;
	}
}

CAttackAction* CAttackAction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttackAction* pInstance = new CAttackAction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAttackAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAttackAction* CAttackAction::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CAttackAction* pInstance = new CAttackAction(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAttackAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAttackAction::Free()
{
	Safe_Release(m_pModel);

	CBehavior::Free();
}
