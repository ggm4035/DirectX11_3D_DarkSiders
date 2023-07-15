#include "stdafx.h"
#include "CMonster_Patrol.h"

#include "CBlackBoard.h"
#include "CRandomLook.h"
#include "CWait.h"
#include "CWalk.h"

#include "CGameInstance.h"

CMonster_Patrol::CMonster_Patrol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSequence(pDevice, pContext)
{
}

CMonster_Patrol::CMonster_Patrol(const CMonster_Patrol& rhs)
	: CSequence(rhs)
{
}

HRESULT CMonster_Patrol::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CSequence::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* isRangeInPlayer = { false };

			pBlackBoard->Get_Type<_bool*>(L"isRangeInPlayer", isRangeInPlayer);

			return !(*isRangeInPlayer);
		});

	return S_OK;
}

HRESULT CMonster_Patrol::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CRandomLook* pRandomLook = dynamic_cast<CRandomLook*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_RandomLook", m_pOwner));
	if (nullptr == pRandomLook)
		return E_FAIL;

	CWalk* pWalk = dynamic_cast<CWalk*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Walk", m_pOwner));
	if (nullptr == pWalk)
		return E_FAIL;

	CWait* pWait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Wait", m_pOwner));
	if (nullptr == pWait)
		return E_FAIL;

	pWalk->Set_LimitTime(2.f);
	pWait->Set_LimitTime(3.f);

	if (FAILED(Assemble_Behavior(L"Tsk_RandomLook", pRandomLook)))
		return E_FAIL;

	if (FAILED(Assemble_Behavior(L"Tsk_Walk", pWalk)))
		return E_FAIL;

	if (FAILED(Assemble_Behavior(L"Tsk_Wait", pWait)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CMonster_Patrol* CMonster_Patrol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Patrol* pInstance = new CMonster_Patrol(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMonster_Patrol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMonster_Patrol* CMonster_Patrol::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CMonster_Patrol* pInstance = new CMonster_Patrol(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CMonster_Patrol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Patrol::Free()
{
	CSequence::Free();
}
