
#include "CPlayerJump.h"

#include "CPlayer.h"
#include "CPlayerAction.h"
#include "CGameInstance.h"

CPlayerJump::CPlayerJump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CPlayerJump::CPlayerJump(const CPlayerJump& rhs)
	: CBehavior(rhs)
{
}

HRESULT CPlayerJump::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	m_pTransform = dynamic_cast<CPlayer*>(m_pOwner)->Get_Transform();
	if (nullptr == m_pTransform)
		return E_FAIL;
	Safe_AddRef(m_pTransform);

	m_pModel = dynamic_cast<CModel*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	return S_OK;
}

HRESULT CPlayerJump::Tick(const _double& TimeDelta)
{
	while (true != m_Qmessage.empty())
	{
		if (true == m_isJump)
			m_isDoubleJump = true;

		m_isJump = true;
		m_isFirst = true;

		m_Qmessage.pop();
	}

	if (false == m_isJump)
		return S_OK;

	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	_bool isJump = { true };

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	switch (pAction->Get_State())
	{
	case Client::CPlayerAction::STATE_RUN:
	case Client::CPlayerAction::STATE_IDLE:
	case Client::CPlayerAction::STATE_JUMP_LAND:
		pAction->Set_State(CPlayerAction::STATE_JUMP);
		isJump = m_pTransform->Jump(0.45f, TimeDelta);
		if (true == m_isFirst)
		{
			pGameInstance->Play_Sound(L"char_war_jump_02.ogg", CSound_Manager::SOUND_PLAYER, 0.3f, true);
			m_isFirst = false;
		}
		break;

	case Client::CPlayerAction::STATE_JUMP:
		if (true == m_isDoubleJump)
		{
			pAction->Set_State(CPlayerAction::STATE_DOUBLE_JUMP);
			dynamic_cast<CPlayer*>(m_pOwner)->Get_Transform()->Reset_TimeAcc();
		}
		isJump = m_pTransform->Jump(0.45f, TimeDelta);
		if (true == m_isFirst)
		{
			pGameInstance->Play_Sound(L"char_war_jumpdouble_02.ogg", CSound_Manager::SOUND_PLAYER, 0.3f, true);
			m_isFirst = false;
		}
		break;

	case Client::CPlayerAction::STATE_DOUBLE_JUMP:
		isJump = m_pTransform->Jump(0.45f, TimeDelta);
		break;
	}

	if (false == isJump)
	{
		pGameInstance->Play_Sound(L"char_war_jump_ledge_02.ogg", CSound_Manager::SOUND_PLAYER, 0.3f, true);
		pAction->Set_State(CPlayerAction::STATE_JUMP_LAND);
		m_isJump = false;
		m_isDoubleJump = false;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CPlayerJump::Reset()
{
	m_isJump = false;
	m_isDoubleJump = false;
	m_isFirst = true;
	m_pTransform->Reset_Jump();
}

CPlayerJump* CPlayerJump::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerJump* pInstance = new CPlayerJump(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CJumpAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CPlayerJump* CPlayerJump::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPlayerJump* pInstance = new CPlayerJump(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CJumpAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerJump::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
