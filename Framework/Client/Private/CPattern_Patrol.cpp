
#include "CPattern_Patrol.h"

#include "CBlackBoard.h"
#include "CRandomLook.h"
#include "CWait.h"
#include "CMove.h"
#include "CAction.h"

#include "CGameInstance.h"

CPattern_Patrol::CPattern_Patrol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSequence(pDevice, pContext)
{
}

CPattern_Patrol::CPattern_Patrol(const CPattern_Patrol& rhs)
	: CSequence(rhs)
{
}

HRESULT CPattern_Patrol::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CSequence::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* isRangeInPlayer = { nullptr };

			pBlackBoard->Get_Type<_bool*>(L"isRangeInPlayer", isRangeInPlayer);

			return !(*isRangeInPlayer);
		});

	return S_OK;
}

HRESULT CPattern_Patrol::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CRandomLook* pRandomLook = dynamic_cast<CRandomLook*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_RandomLook", m_pOwner));
	if (nullptr == pRandomLook)
		return E_FAIL;
	CAction* pAction_Walk = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_Walk)
		return E_FAIL;
	CAction* pAction_Idle = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_Idle)
		return E_FAIL;

	CWait* pWait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Wait", m_pOwner));
	if (nullptr == pWait)
		return E_FAIL;
	CMove* pMove = dynamic_cast<CMove*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Move", m_pOwner));
	if (nullptr == pMove)
		return E_FAIL;

	pAction_Walk->Bind_AnimationTag("Walk");
	pAction_Idle->Bind_AnimationTag("Idle");
	pMove->Bind_Move_Force(0.5f);
	pMove->Set_Timer(2.f);
	pWait->Set_Timer(3.f);

	if (FAILED(Assemble_Behavior(L"Tsk_RandomLook", pRandomLook)))
		return E_FAIL;
	if (FAILED(Assemble_Behavior(L"Action_Walk", pAction_Walk)))
		return E_FAIL;
	if (FAILED(Assemble_Behavior(L"Action_Idle", pAction_Idle)))
		return E_FAIL;

	if (FAILED(pAction_Walk->Assemble_Behavior(L"Tsk_Move", pMove)))
		return E_FAIL;
	if (FAILED(pAction_Idle->Assemble_Behavior(L"Tsk_Wait", pWait)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CPattern_Patrol* CPattern_Patrol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPattern_Patrol* pInstance = new CPattern_Patrol(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMonster_Patrol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPattern_Patrol* CPattern_Patrol::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPattern_Patrol* pInstance = new CPattern_Patrol(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CMonster_Patrol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPattern_Patrol::Free()
{
	CSequence::Free();
}
