#include "CBoss_Attack.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CAction.h"
#include "CSequence.h"

CBoss_Attack::CBoss_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSelector(pDevice, pContext)
{
}

CBoss_Attack::CBoss_Attack(const CBoss_Attack& rhs)
	: CSelector(rhs)
{
}

HRESULT CBoss_Attack::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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
			CGameObject3D::HITSTATE* pCurHitState;
			pBlackBoard->Get_Type(L"eCurHitState", pCurHitState);

			if (CGameObject3D::NONE == *pCurHitState)
				return true;

			return false;
		});


	return S_OK;
}

HRESULT CBoss_Attack::Tick(const _double& TimeDelta)
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

HRESULT CBoss_Attack::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CRandomSelector* pRandom_Attack = dynamic_cast<CRandomSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"RandomSelector", m_pOwner));
	if (nullptr == pRandom_Attack)
		return E_FAIL;

	if (FAILED(Assemble_Behavior(L"Random_Attack", pRandom_Attack)))
		return E_FAIL;

	for (auto& strAnimTag : m_AttackAnimTagList)
	{
		CAction* pAction_Attack = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
		if (nullptr == pAction_Attack)
			return E_FAIL;

		pAction_Attack->Bind_AnimationTag(strAnimTag);
		if (FAILED(pRandom_Attack->Assemble_Behavior(pGameInstance->strToWStr(strAnimTag), pAction_Attack)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CBoss_Attack* CBoss_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Attack* pInstance = new CBoss_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBoss_Attack_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBoss_Attack* CBoss_Attack::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CBoss_Attack* pInstance = new CBoss_Attack(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CBoss_Attack_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Attack::Free()
{
	if (true == m_isCloned)
		Safe_Release(m_pModel);

	CSelector::Free();
}
