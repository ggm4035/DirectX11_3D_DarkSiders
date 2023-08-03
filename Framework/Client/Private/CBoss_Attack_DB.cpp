#include "CBoss_Attack_DB.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CAction.h"
#include "CSequence.h"

CBoss_Attack_DB::CBoss_Attack_DB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CBoss_Attack_DB::CBoss_Attack_DB(const CBoss_Attack_DB& rhs)
	: CBehavior(rhs)
{
}

HRESULT CBoss_Attack_DB::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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
			CGameObject3D::HITSTATE* pCurHitState;
			pBlackBoard->Get_Type(L"eCurHitState", pCurHitState);

			if (CGameObject3D::NONE == *pCurHitState)
				return true;

			return false;
		});

	return S_OK;
}

HRESULT CBoss_Attack_DB::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
	{
		m_isFirst = true;
		return BEHAVIOR_FAIL;
	}

	if (true == m_isFirst)
	{
		CGameInstance::GetInstance()->Play_Sound(L"en_hollowlord_atk_doubleslam_vo_02.ogg", CSound_Manager::SOUND_ENEMY, 0.5f);
		m_pModel->Change_Animation("Attack_1");
		m_isFirst = false;
	}

	m_fTimeAcc += TimeDelta;
	if (3.f <= m_fTimeAcc)
	{
		CGameInstance::GetInstance()->Play_Sound(L"en_hollowlord_atk_slam_explosion_02.ogg", CSound_Manager::SOUND_ENEMY, 0.5f, true);
		m_fTimeAcc = 0.f;
	}

	if (true == m_pModel->isAbleChangeAnimation() ||
		true == m_pModel->isFinishedAnimation())
	{
		m_isFirst = true;
		m_fTimeAcc = 0.f;

		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

CBoss_Attack_DB* CBoss_Attack_DB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Attack_DB* pInstance = new CBoss_Attack_DB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBoss_Attack_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBoss_Attack_DB* CBoss_Attack_DB::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CBoss_Attack_DB* pInstance = new CBoss_Attack_DB(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CBoss_Attack_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Attack_DB::Free()
{
	if (true == m_isCloned)
		Safe_Release(m_pModel);

	CBehavior::Free();
}
