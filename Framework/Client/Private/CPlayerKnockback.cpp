#include "CPlayerKnockback.h"

#include "CPlayerAction.h"
#include "CPlayer.h"

#include "CGameInstance.h"
#include "CSequence.h"
#include "CBlackBoard.h"
#include "CAction.h"
#include "CMove.h"
#include "CJump.h"
#include "CWeapon.h"

CPlayerKnockback::CPlayerKnockback(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CPlayerKnockback::CPlayerKnockback(const CPlayerKnockback& rhs)
	: CBehavior(rhs)
{
}

HRESULT CPlayerKnockback::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerKnockback::Tick(const _double& TimeDelta)
{
	CTransform* pTransform = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type(L"pTransform", pTransform)))
		return E_FAIL;

	if (true == m_isFirst)
	{
		CPlayer* pOwner = dynamic_cast<CPlayer*>(m_pOwner);
		dynamic_cast<CWeapon*>(pOwner->Get_Parts(L"Weapon"))->Off_SwordTrail();
		pOwner->Get_Collider(L"Col_Attack")->Set_Enable(false);
		pOwner->Get_Collider(L"Col_WheelWind")->Set_Enable(false);

		m_isFirst = false;
	}

	XMStoreFloat3(&m_vDirection, pTransform->Get_State(CTransform::STATE_LOOK));

	m_pBlackBoard->Set_Type(L"vDirection", m_vDirection);

	HRESULT hr = S_OK;

	for (auto& BehaviorDesc : m_BehaviorList)
		hr = BehaviorDesc.pBehavior->Tick(TimeDelta);

	_vector vLook = pTransform->Get_State(CTransform::STATE_LOOK);
	pTransform->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(XMVectorSetY(vLook, 0.f)));

	if (hr == BEHAVIOR_SUCCESS)
		m_isFirst = true;

	return hr;
}

HRESULT CPlayerKnockback::AssembleBehaviors()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSequence* pSequence = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence", m_pOwner));
	if (nullptr == pSequence)
		return E_FAIL;

	CAction* pAction_Loop = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_Loop)
		return E_FAIL;
	CAction* pAction_Exit = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_Exit)
		return E_FAIL;

	CMove* pMove = dynamic_cast<CMove*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Move", m_pOwner));
	if (nullptr == pMove)
		return E_FAIL;
	CJump* pJump = dynamic_cast<CJump*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Jump", m_pOwner));
	if (nullptr == pJump)
		return E_FAIL;

	pAction_Loop->Bind_AnimationTag("Knockback_Loop");
	pAction_Exit->Bind_AnimationTag("Knockback_Exit");

	pMove->Bind_Move_Force(-2.f);
	pJump->Bind_Jump_Force(0.4f);

	if (FAILED(Assemble_Behavior(L"Sequence", pSequence)))
		return E_FAIL;

	if (FAILED(pSequence->Assemble_Behavior(L"Action_Loop", pAction_Loop)))
		return E_FAIL;
	if (FAILED(pSequence->Assemble_Behavior(L"Action_Exit", pAction_Exit)))
		return E_FAIL;

	if (FAILED(pAction_Loop->Assemble_Behavior(L"Tsk_Move", pMove)))
		return E_FAIL;
	if (FAILED(pAction_Loop->Assemble_Behavior(L"Tsk_Jump", pJump)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CPlayerKnockback* CPlayerKnockback::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerKnockback* pInstance = new CPlayerKnockback(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMoveAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CPlayerKnockback* CPlayerKnockback::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPlayerKnockback* pInstance = new CPlayerKnockback(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CMoveAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerKnockback::Free()
{
	CBehavior::Free();
}
