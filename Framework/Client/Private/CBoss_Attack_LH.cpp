#include "CBoss_Attack_LH.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CAction.h"
#include "CSequence.h"

CBoss_Attack_LH::CBoss_Attack_LH(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CBoss_Attack_LH::CBoss_Attack_LH(const CBoss_Attack_LH& rhs)
	: CBehavior(rhs)
{
}

HRESULT CBoss_Attack_LH::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	CGameObject3D* pGameObject = dynamic_cast<CGameObject3D*>(m_pOwner);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_pModel = dynamic_cast<CModel*>(pGameObject->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	/* ÄðÅ¸ÀÓ */
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_float* pTimeAcc = { nullptr };
			pBlackBoard->Get_Type(L"fTimeAcc", pTimeAcc);
			if (nullptr == pTimeAcc)
				return false;

			_float fInterval = *pTimeAcc - m_fPreTimeAcc;
			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CHealth* pHealth = { nullptr };
			pBlackBoard->Get_Type(L"pHealth", pHealth);
			if (nullptr == pHealth)
				return false;

			if (CHealth::HIT_NONE == pHealth->Get_Current_HitState())
				return true;

			return false;
		});

	return S_OK;
}

HRESULT CBoss_Attack_LH::Tick(const _double& TimeDelta)
{
	m_fTimeAcc += TimeDelta;

	if (false == Check_Decorations())
	{
		m_isFirst = true;
		return BEHAVIOR_FAIL;
	}

	if (true == m_isFirst)
	{
		CGameInstance::GetInstance()->Play_Sound(L"en_hollowlord_atk_swipe_L_vo_03.ogg", CSound_Manager::SOUND_BOSSEFFECT_1, 0.5f);
		m_pModel->Change_Animation("Attack_2");
		m_isFirst = false;
	}

	if (false == m_isPlaySound && 2.7f < m_fTimeAcc)
	{
		if (FAILED(CGameInstance::GetInstance()->Play_Sound(L"en_steamroller_attack_impact_02.ogg", CSound_Manager::SOUND_BOSSEFFECT_2, 0.7f, true)))
			return E_FAIL;
		m_isPlaySound = true;
	}

	if (true == m_pModel->isAbleChangeAnimation() ||
		true == m_pModel->isFinishedAnimation())
	{
		m_isFirst = true;
		m_isPlaySound = false;
		m_fTimeAcc = 0.f;

		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

CBoss_Attack_LH* CBoss_Attack_LH::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Attack_LH* pInstance = new CBoss_Attack_LH(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBoss_Attack_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBoss_Attack_LH* CBoss_Attack_LH::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CBoss_Attack_LH* pInstance = new CBoss_Attack_LH(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CBoss_Attack_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Attack_LH::Free()
{
	if (true == m_isCloned)
		Safe_Release(m_pModel);

	CBehavior::Free();
}
