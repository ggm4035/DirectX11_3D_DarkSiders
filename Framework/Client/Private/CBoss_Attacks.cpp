#include "CBoss_Attacks.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CAction.h"
#include "CSequence.h"
#include "CBoss_Attack_DB.h"
#include "CBoss_Attack_LH.h"
#include "CBoss_Attack_LHS.h"
#include "CBoss_Attack_RH.h"
#include "CBoss_Attack_RHS.h"

CBoss_Attacks::CBoss_Attacks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSelector(pDevice, pContext)
{
}

CBoss_Attacks::CBoss_Attacks(const CBoss_Attacks& rhs)
	: CSelector(rhs)
{
}

HRESULT CBoss_Attacks::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CSelector::Initialize(iLevelIndex, pOwner, pArg)))
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

HRESULT CBoss_Attacks::Tick(const _double& TimeDelta)
{
	HRESULT hr = CSelector::Tick(TimeDelta);

	_float* pTimeAcc = { nullptr };
	m_pBlackBoard->Get_Type(L"fTimeAcc", pTimeAcc);
	if (nullptr == pTimeAcc)
		return E_FAIL;

	if (BEHAVIOR_SUCCESS == hr)
		m_fPreTimeAcc = *pTimeAcc;

	return hr;
}

HRESULT CBoss_Attacks::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CRandomSelector* pRandom_Attack = dynamic_cast<CRandomSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"RandomSelector", m_pOwner));
	if (nullptr == pRandom_Attack)
		return E_FAIL;

	CBoss_Attack_DB* pAttack_DB = dynamic_cast<CBoss_Attack_DB*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Boss_Attack_DB", m_pOwner));
	if (nullptr == pAttack_DB)
		return E_FAIL;
	CBoss_Attack_LH* pAttack_LH = dynamic_cast<CBoss_Attack_LH*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Boss_Attack_LH", m_pOwner));
	if (nullptr == pAttack_LH)
		return E_FAIL;
	CBoss_Attack_LHS* pAttack_LHS = dynamic_cast<CBoss_Attack_LHS*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Boss_Attack_LHS", m_pOwner));
	if (nullptr == pAttack_LHS)
		return E_FAIL;
	CBoss_Attack_RH* pAttack_RH = dynamic_cast<CBoss_Attack_RH*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Boss_Attack_RH", m_pOwner));
	if (nullptr == pAttack_RH)
		return E_FAIL;
	CBoss_Attack_RHS* pAttack_RHS = dynamic_cast<CBoss_Attack_RHS*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Boss_Attack_RHS", m_pOwner));
	if (nullptr == pAttack_RHS)
		return E_FAIL;

	if (FAILED(Assemble_Behavior(L"Random_Attack", pRandom_Attack)))
		return E_FAIL;

	if (FAILED(pRandom_Attack->Assemble_Behavior(L"Attack_DB", pAttack_DB)))
		return E_FAIL;
	if (FAILED(pRandom_Attack->Assemble_Behavior(L"Attack_LH", pAttack_LH)))
		return E_FAIL;
	if (FAILED(pRandom_Attack->Assemble_Behavior(L"Attack_LHS", pAttack_LHS)))
		return E_FAIL;
	if (FAILED(pRandom_Attack->Assemble_Behavior(L"Attack_RH", pAttack_RH)))
		return E_FAIL;
	if (FAILED(pRandom_Attack->Assemble_Behavior(L"Attack_RHS", pAttack_RHS)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CBoss_Attacks* CBoss_Attacks::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Attacks* pInstance = new CBoss_Attacks(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBoss_Attack_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBoss_Attacks* CBoss_Attacks::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CBoss_Attacks* pInstance = new CBoss_Attacks(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CBoss_Attack_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Attacks::Free()
{
	if (true == m_isCloned)
		Safe_Release(m_pModel);

	CSelector::Free();
}
