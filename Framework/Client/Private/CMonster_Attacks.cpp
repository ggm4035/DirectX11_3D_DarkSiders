#include "stdafx.h"
#include "CMonster_Attacks.h"

#include "CAttack_1.h"
#include "CAttack_2.h"
#include "CAttack_3.h"

#include "CGameInstance.h"
#include "CBlackBoard.h"
#include "CModel.h"

CMonster_Attacks::CMonster_Attacks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSelector(pDevice, pContext)
{
}

CMonster_Attacks::CMonster_Attacks(const CMonster_Attacks& rhs)
	:CSelector(rhs)
{
}

HRESULT CMonster_Attacks::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CSelector::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Attacks::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CAttack_1* pAttack_1 = dynamic_cast<CAttack_1*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Attack_1", m_pOwner));
	if (nullptr == pAttack_1)
		return E_FAIL;

	CAttack_2* pAttack_2 = dynamic_cast<CAttack_2*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Attack_2", m_pOwner));
	if (nullptr == pAttack_2)
		return E_FAIL;

	CAttack_3* pAttack_3 = dynamic_cast<CAttack_3*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Attack_3", m_pOwner));
	if (nullptr == pAttack_3)
		return E_FAIL;

	if (FAILED(Assemble_Behavior(L"Tsk_Attack_1", pAttack_1)))
		return E_FAIL;

	if (FAILED(Assemble_Behavior(L"Tsk_Attack_2", pAttack_2)))
		return E_FAIL;

	if (FAILED(Assemble_Behavior(L"Tsk_Attack_3", pAttack_3)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CMonster_Attacks* CMonster_Attacks::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Attacks* pInstance = new CMonster_Attacks(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMonster_Attacks");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMonster_Attacks* CMonster_Attacks::Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg)
{
	CMonster_Attacks* pInstance = new CMonster_Attacks(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CMonster_Attacks");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Attacks::Free()
{
	CSelector::Free();
}
