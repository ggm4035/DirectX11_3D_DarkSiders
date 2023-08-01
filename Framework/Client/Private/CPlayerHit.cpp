
#include "CPlayerHit.h"

#include "CPlayerAction.h"
#include "CPlayer.h"
#include "CModel.h"
#include "CCollider.h"

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

	m_pPlayer = dynamic_cast<CPlayer*>(m_pOwner);

	m_pTransform = m_pPlayer->Get_Transform();
	if (nullptr == m_pTransform)
		return E_FAIL;
	Safe_AddRef(m_pTransform);

	m_pModel = dynamic_cast<CModel*>(m_pPlayer->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	return S_OK;
}

HRESULT CPlayerHit::Tick(const _double& TimeDelta)
{
	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	if (CPlayerAction::STATE_WHEEL == pAction->Get_State() ||
		CPlayerAction::STATE_LEAP == pAction->Get_State() ||
		CPlayerAction::STATE_KNOCKBACK == pAction->Get_State())
	{
		m_pPlayer->Set_CurHitState(CPlayer::NONE);
	}

	CPlayer::HITSTATE eCurHitState = m_pPlayer->Get_CurHitState();

	switch (eCurHitState)
	{
	case CGameObject3D::HIT:
		pAction->Set_State(CPlayerAction::STATE_HIT);
		m_pPlayer->Set_CurHitState(CPlayer::HITTING);
		m_pPlayer->Get_Collider(L"Col_Attack")->Set_Enable(false);
		m_pPlayer->Get_Collider(L"Col_WheelWind")->Set_Enable(false);
		break;

	case CGameObject3D::HITTING:
		if (true == m_pModel->isAbleChangeAnimation() ||
			true == m_pModel->isFinishedAnimation())
		{
			pAction->Set_State(CPlayerAction::STATE_IDLE);
			pAction->On_SuperArmor();
			m_pPlayer->Set_CurHitState(CPlayer::NONE);
		}
		break;
	}

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
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
