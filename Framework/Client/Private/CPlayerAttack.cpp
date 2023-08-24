
#include "CPlayerAttack.h"

#include "CGameInstance.h"
#include "CPlayerAction.h"
#include "CBlackBoard.h"
#include "CPlayer.h"
#include "CWeapon.h"

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

	m_pWeapon = dynamic_cast<CWeapon*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Parts(L"Weapon"));
	if (nullptr == m_pWeapon)
		return E_FAIL;
	Safe_AddRef(m_pWeapon);

	return S_OK;
}

HRESULT CPlayerAttack::Tick(const _double& TimeDelta)
{
	CModel* pModel = { nullptr };

	if(FAILED(m_pBlackBoard->Get_Type(L"pModel", pModel)))
		return E_FAIL;

	if (true == pModel->isFinishedAnimation() &&
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

			if (true == pModel->isLoopAnimation() ||
				true == pModel->isAbleChangeAnimation())
				m_iCombo = (3 < ++m_iCombo) ? -1 : m_iCombo;

			break;
		}
		m_Qmessage.pop();
	}

	if (m_iCombo == m_iPreCombo)
		return S_OK;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);
	
	m_pWeapon->On_SwordTrail();

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
			pGameInstance->Play_Sound(L"char_war_attack_2_02.ogg", CSound_Manager::SOUND_PLAYER, 0.35f, true);
			break;
		case 1:
			pAction->Set_State(CPlayerAction::STATE_LATK_2);
			pGameInstance->Play_Sound(L"char_war_attack_3_02.ogg", CSound_Manager::SOUND_PLAYER, 0.35f, true);
			break;
		case 2:
			pAction->Set_State(CPlayerAction::STATE_LATK_3);
			pGameInstance->Play_Sound(L"char_war_attack_3_02.ogg", CSound_Manager::SOUND_PLAYER, 0.35f, true);
			break;
		case 3:
			pAction->Set_State(CPlayerAction::STATE_LATK_4);
			pGameInstance->Play_Sound(L"char_war_attack_4_b_02.ogg", CSound_Manager::SOUND_PLAYER, 0.35f, true);
			m_iCombo = -1;
			break;
		}
		break;

	case Client::CPlayerAction::STATE_LATK_1:
		pGameInstance->Play_Sound(L"char_war_attack_2_02.ogg", CSound_Manager::SOUND_PLAYER, 0.35f, true);
		pAction->Set_State(CPlayerAction::STATE_LATK_2);
		break;
	case Client::CPlayerAction::STATE_LATK_2:
		pGameInstance->Play_Sound(L"char_war_attack_3_02.ogg", CSound_Manager::SOUND_PLAYER, 0.35f, true);
		pAction->Set_State(CPlayerAction::STATE_LATK_3);
		break;
	case Client::CPlayerAction::STATE_LATK_3:
		pGameInstance->Play_Sound(L"char_war_attack_4_b_02.ogg", CSound_Manager::SOUND_PLAYER, 0.35f, true);
		pAction->Set_State(CPlayerAction::STATE_LATK_4);
		break;
	case Client::CPlayerAction::STATE_LATK_4:
		pGameInstance->Play_Sound(L"char_war_attack_1_02.ogg", CSound_Manager::SOUND_PLAYER, 0.35f, true);
		pAction->Set_State(CPlayerAction::STATE_LATK_1);
		break;
	}

	Safe_Release(pGameInstance);
	m_iPreCombo = m_iCombo;

	return S_OK;
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
	{
		Safe_Release(m_pWeapon);
	}

	CBehavior::Free();
}
