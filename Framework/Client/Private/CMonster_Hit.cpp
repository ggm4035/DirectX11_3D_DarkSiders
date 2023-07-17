#include "CMonster_Hit.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CHit.h"

CMonster_Hit::CMonster_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSequence(pDevice, pContext)
{
}

CMonster_Hit::CMonster_Hit(const CMonster_Hit& rhs)
	: CSequence(rhs)
{
}

HRESULT CMonster_Hit::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CSequence::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CGameObject3D::HITSTATE eCurHitState;
			pBlackBoard->Get_Type(L"eCurHitState", eCurHitState);

			if (CGameObject3D::NONE == eCurHitState)
				return false;
			else
				return true;
		});

	return S_OK;
}

HRESULT CMonster_Hit::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CHit* pHit = dynamic_cast<CHit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Hit", m_pOwner));
	if (nullptr == pHit)
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CMonster_Hit* CMonster_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Hit* pInstance = new CMonster_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMonster_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMonster_Hit* CMonster_Hit::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CMonster_Hit* pInstance = new CMonster_Hit(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CMonster_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Hit::Free()
{
	CSequence::Free();
}
