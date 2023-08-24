#include "CPlayerHit.h"

#include "CGameInstance.h"
#include "CPlayerKnockback.h"
#include "CPlayerAction.h"
#include "CBlackBoard.h"
#include "CWeapon.h"
#include "CPlayer.h"

CPlayerHit::CPlayerHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CPlayerHit::CPlayerHit(const CPlayerHit& rhs)
	: CBehavior(rhs)
{
}

HRESULT CPlayerHit::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerHit::Tick(const _double& TimeDelta)
{
	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	CHealth* pHealth = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type(L"pHealth", pHealth)))
		return E_FAIL;
	CTransform* pTransform = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type(L"pTransform", pTransform)))
		return E_FAIL;

	if (CPlayerAction::STATE_WHEEL == pAction->Get_State() ||
		CPlayerAction::STATE_LEAP == pAction->Get_State())
	{
		pHealth->Set_HitState(CHealth::HIT_NONE);
	}

	CHealth::HITSTATE eCurHitState = pHealth->Get_Current_HitState();

	CModel* pModel = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type(L"pModel", pModel)))
		return E_FAIL;

	CPlayer* pOwner = dynamic_cast<CPlayer*>(m_pOwner);
	if (nullptr == pOwner)
		return E_FAIL;

	switch (eCurHitState)
	{
	case CHealth::HIT_ENTER:
		_vector vLook = pTransform->Get_State(CTransform::STATE_LOOK);
		pTransform->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(XMVectorSetY(vLook, 0.f)));
		pTransform->Organize_From_Look();

		dynamic_cast<CWeapon*>(pOwner->Get_Parts(L"Weapon"))->Off_SwordTrail();
		pAction->Set_State(CPlayerAction::STATE_HIT);
		pHealth->Set_HitState(CHealth::HIT_STAY);
		pOwner->Get_Collider(L"Col_Attack")->Set_Enable(false);
		pOwner->Get_Collider(L"Col_WheelWind")->Set_Enable(false);
		CGameInstance::GetInstance()->Play_Sound(L"War_Effort_06.ogg", CSound_Manager::SOUND_PLAYER, 0.4f, true);
		break;

	case CHealth::HIT_KNOCKBACK:
		dynamic_cast<CWeapon*>(pOwner->Get_Parts(L"Weapon"))->Off_SwordTrail();
		pHealth->Set_HitState(CHealth::HIT_STAY);
		pOwner->Get_Collider(L"Col_Attack")->Set_Enable(false);
		pOwner->Get_Collider(L"Col_WheelWind")->Set_Enable(false);
		CGameInstance::GetInstance()->Play_Sound(L"War_Effort_06.ogg", CSound_Manager::SOUND_PLAYER, 0.4f, true);
		break;

	case CHealth::HIT_STAY:
		m_fHitTimeAcc += TimeDelta;

		if (CPlayerAction::STATE_KNOCKBACK == pAction->Get_State())
			m_pKnockbackAction->Tick(TimeDelta);

		if (true == pModel->isAbleChangeAnimation() ||
			true == pModel->isFinishedAnimation())
		{
			if (CPlayerAction::STATE_KNOCKBACK == pAction->Get_State())
				pAction->Reset_Jump();

			pAction->Set_State(CPlayerAction::STATE_IDLE);
			pAction->On_SuperArmor();
			pHealth->Set_HitState(CHealth::HIT_NONE);
			m_fHitTimeAcc = 0.f;
		}
		break;
	}

	return S_OK;
}

HRESULT CPlayerHit::AssembleBehaviors()
{
	CBehavior* pAction = dynamic_cast<CPlayerKnockback*>(CGameInstance::GetInstance()
		->Clone_Component(LEVEL_STATIC, L"PlayerKnockback", m_pOwner));
	if (nullptr == pAction)
		return E_FAIL;

	if (FAILED(Assemble_Behavior(L"PlayerKnockback", pAction)))
		return E_FAIL;

	m_pKnockbackAction = dynamic_cast<CPlayerKnockback*>(pAction);
	if (nullptr == m_pKnockbackAction)
		return E_FAIL;

	if (FAILED(m_pKnockbackAction->AssembleBehaviors()))
		return E_FAIL;

	return S_OK;
}

CPlayerHit* CPlayerHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerHit* pInstance = new CPlayerHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMoveAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CPlayerHit* CPlayerHit::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPlayerHit* pInstance = new CPlayerHit(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CMoveAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerHit::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pKnockbackAction);
	}

	CBehavior::Free();
}
