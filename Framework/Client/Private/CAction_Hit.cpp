#include "CAction_Hit.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CHit.h"
#include "CDead.h"

CAction_Hit::CAction_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSequence(pDevice, pContext)
{
}

CAction_Hit::CAction_Hit(const CAction_Hit& rhs)
	: CSequence(rhs)
{
}

HRESULT CAction_Hit::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CSequence::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CGameObject3D::HITSTATE* pCurHitState = { nullptr };
			pBlackBoard->Get_Type(L"eCurHitState", pCurHitState);
			if (nullptr == pCurHitState)
				return false;

			if (CGameObject3D::NONE == *pCurHitState)
				return false;
			else
				return true;
		});

	return S_OK;
}

HRESULT CAction_Hit::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CHit* pHit = dynamic_cast<CHit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Hit", m_pOwner));
	if (nullptr == pHit)
		return E_FAIL;

	CDead* pDead = dynamic_cast<CDead*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Dead", m_pOwner));
	if (nullptr == pDead)
		return E_FAIL;

	if (FAILED(Assemble_Behavior(L"Tsk_Hit", pHit)))
		return E_FAIL;

	if (FAILED(Assemble_Behavior(L"Tsk_Dead", pDead)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CAction_Hit* CAction_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAction_Hit* pInstance = new CAction_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMonster_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAction_Hit* CAction_Hit::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CAction_Hit* pInstance = new CAction_Hit(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CMonster_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAction_Hit::Free()
{
	CSequence::Free();
}
