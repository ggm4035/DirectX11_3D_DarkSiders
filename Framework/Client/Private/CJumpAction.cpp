#include "stdafx.h"
#include "CJumpAction.h"

#include "CPlayer.h"
#include "CPlayerAction.h"
#include "CGameInstance.h"

CJumpAction::CJumpAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CJumpAction::CJumpAction(const CJumpAction& rhs)
	: CBehavior(rhs)
{
}

HRESULT CJumpAction::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

HRESULT CJumpAction::Tick(const _double& TimeDelta)
{
	while (true != m_Qmessage.empty())
	{
		if (true == m_isJump)
			m_isDoubleJump = true;

		m_isJump = true;

		m_Qmessage.pop();
	}

	if (false == m_isJump)
		return S_OK;

	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	_bool isJump = { true };

	switch (pAction->Get_State())
	{
	case Client::CPlayerAction::STATE_IDLE:
		pAction->Set_State(CPlayerAction::STATE_JUMP);
		isJump = m_pTransform->Jump(0.45f, TimeDelta);
		break;

	case Client::CPlayerAction::STATE_RUN:
		pAction->Set_State(CPlayerAction::STATE_JUMP);
		isJump = m_pTransform->Jump(0.45f, TimeDelta);
		break;

	case Client::CPlayerAction::STATE_JUMP_LAND:
		pAction->Set_State(CPlayerAction::STATE_JUMP);
		isJump = m_pTransform->Jump(0.45f, TimeDelta);
		break;

	case Client::CPlayerAction::STATE_JUMP:
		if (true == m_isDoubleJump)
		{
			pAction->Set_State(CPlayerAction::STATE_DOUBLE_JUMP);
			dynamic_cast<CPlayer*>(m_pOwner)->Get_Transform()->Reset_TimeAcc();
		}
		isJump = m_pTransform->Jump(0.45f, TimeDelta);
		break;

	case Client::CPlayerAction::STATE_DOUBLE_JUMP:
		isJump = m_pTransform->Jump(0.45f, TimeDelta);
		break;
	}

	if (false == isJump)
	{
		pAction->Set_State(CPlayerAction::STATE_JUMP_LAND);
		m_isJump = false;
		m_isDoubleJump = false;
	}

	return S_OK;
}

CJumpAction* CJumpAction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJumpAction* pInstance = new CJumpAction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CJumpAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CJumpAction* CJumpAction::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CJumpAction* pInstance = new CJumpAction(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CJumpAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJumpAction::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
