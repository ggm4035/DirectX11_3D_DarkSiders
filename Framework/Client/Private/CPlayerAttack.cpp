
#include "CPlayerAttack.h"

#include "CGameInstance.h"
#include "CPlayerAction.h"
#include "CPlayer.h"
#include "CModel.h"

CPlayerAttack::CPlayerAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CPlayerAttack::CPlayerAttack(const CPlayerAttack& rhs)
	: CBehavior(rhs)
{
}

HRESULT CPlayerAttack::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	m_pModel = dynamic_cast<CModel*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	return S_OK;
}

HRESULT CPlayerAttack::Tick(const _double& TimeDelta)
{
	if (true == m_pModel->isFinishedAnimation() &&
		true == m_Qmessage.empty())
		m_iCombo = -1;

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
	case Client::CPlayerAttack::TYPE_LIGHT:
		LightAttackCombo();
		break;

	case Client::CPlayerAttack::TYPE_HEAVY:
		HeavyAttackCombo();
		break;
	}

	m_iPreCombo = m_iCombo;

	return S_OK;
}

void CPlayerAttack::LightAttackCombo()
{
	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	switch (pAction->Get_State())
	{
	case Client::CPlayerAction::STATE_IDLE:
	case Client::CPlayerAction::STATE_RUN:
	case Client::CPlayerAction::STATE_DASH:
	case Client::CPlayerAction::STATE_JUMP:
	case Client::CPlayerAction::STATE_DOUBLE_JUMP:
		switch (m_iCombo)
		{
		case 0:
			pAction->Set_State(CPlayerAction::STATE_LATK_1);
			break;
		case 1:
			pAction->Set_State(CPlayerAction::STATE_LATK_2);
			break;
		case 2:
			pAction->Set_State(CPlayerAction::STATE_LATK_3);
			break;
		case 3:
			pAction->Set_State(CPlayerAction::STATE_LATK_4);
			m_iCombo = -1;
			break;
		}
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

void CPlayerAttack::HeavyAttackCombo()
{
	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	switch (pAction->Get_State())
	{
	case Client::CPlayerAction::STATE_IDLE:
	case Client::CPlayerAction::STATE_RUN:
	case Client::CPlayerAction::STATE_DASH:
	case Client::CPlayerAction::STATE_JUMP:
	case Client::CPlayerAction::STATE_DOUBLE_JUMP:
		switch (m_iCombo)
		{
		case 0:
			pAction->Set_State(CPlayerAction::STATE_HATK_1);
			break;
		case 1:
			pAction->Set_State(CPlayerAction::STATE_HATK_2);
			break;
		case 2:
			pAction->Set_State(CPlayerAction::STATE_HATK_3);
			m_iCombo = -1;
			break;
		}
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

CPlayerAttack* CPlayerAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerAttack* pInstance = new CPlayerAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAttackAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CPlayerAttack* CPlayerAttack::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPlayerAttack* pInstance = new CPlayerAttack(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAttackAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerAttack::Free()
{
	if (true == m_isCloned)
		Safe_Release(m_pModel);

	CBehavior::Free();
}
