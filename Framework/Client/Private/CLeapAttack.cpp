
#include "CLeapAttack.h"

#include "CGameInstance.h"
#include "CPlayerAction.h"
#include "CPlayer.h"
#include "CWeapon.h"
#include "CModel.h"
#include "CStone_Effect.h"

CLeapAttack::CLeapAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CLeapAttack::CLeapAttack(const CLeapAttack& rhs)
	: CBehavior(rhs)
{
}

HRESULT CLeapAttack::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	m_pModel = dynamic_cast<CModel*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	m_pTransform = dynamic_cast<CTransform*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Transform());
	if (nullptr == m_pTransform)
		return E_FAIL;
	Safe_AddRef(m_pTransform);

	m_pEffect = dynamic_cast<CStone_Effect*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Component(L"Com_Effect"));
	if (nullptr == m_pEffect)
		return E_FAIL;
	Safe_AddRef(m_pEffect);

	return S_OK;
}

HRESULT CLeapAttack::Tick(const _double& TimeDelta)
{
	m_fCoolTime += TimeDelta;

	if (false == m_isPlay)
		return S_OK;

	if (true == m_isFirst)
	{
		dynamic_cast<CWeapon*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Parts(L"Weapon"))->On_SwordTrail();
		m_pTransform->Set_On_Navigation(false);
		m_pModel->Change_Animation("Atk_Heavy_03");
		CGameInstance::GetInstance()->Play_Sound(L"char_war_attack_heavy_3.ogg", CSound_Manager::SOUND_PLAYER, 0.5f, true);
		m_isFirst = false;
		m_fTimeAcc = 0.f;
	}

	m_fTimeAcc += TimeDelta;

	if (1.0f <= m_fTimeAcc && false == m_isRenderEffect)
	{
		m_pEffect->Render_Effect(m_pTransform->Get_State(CTransform::STATE_POSITION));
		m_isRenderEffect = true;
	}

	if (true == m_pModel->isFinishedAnimation() ||
		true == m_pModel->isAbleChangeAnimation())
	{
		dynamic_cast<CWeapon*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Parts(L"Weapon"))->Off_SwordTrail();
		Reset();
	}

	return S_OK;
}

void CLeapAttack::Reset()
{
	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	pAction->Set_State(CPlayerAction::STATE_IDLE);
	dynamic_cast<CPlayer*>(m_pOwner)->Get_Collider(L"Col_WheelWind")->Set_Enable(false);
	dynamic_cast<CPlayer*>(m_pOwner)->Get_Collider(L"Col_Attack")->Set_Enable(false);
	m_pTransform->Set_On_Navigation(true);
	m_isPlay = false;
	m_isFirst = true;
	m_isRenderEffect = false;
	m_fCoolTime = 0.f;
	m_fTimeAcc = 0.f;
}

void CLeapAttack::Play()
{
	if (false == m_isPlay && 3.f <= m_fCoolTime)
	{
		CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

		pAction->Set_State(CPlayerAction::STATE_LEAP);

		m_isPlay = true;
	}
}

CLeapAttack* CLeapAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLeapAttack* pInstance = new CLeapAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAttackAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CLeapAttack* CLeapAttack::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CLeapAttack* pInstance = new CLeapAttack(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAttackAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLeapAttack::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pEffect);
		Safe_Release(m_pModel);
		Safe_Release(m_pTransform);
	}
	CBehavior::Free();
}
