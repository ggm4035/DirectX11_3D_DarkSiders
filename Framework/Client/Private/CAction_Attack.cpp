#include "CAction_Attack.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CAttack.h"
#include "CFollow.h"

CAction_Attack::CAction_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSelector(pDevice, pContext)
{
}

CAction_Attack::CAction_Attack(const CAction_Attack& rhs)
	: CSelector(rhs)
{
}

HRESULT CAction_Attack::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CGameObject3D::HITSTATE* pCurHitState;
			pBlackBoard->Get_Type(L"eCurHitState", pCurHitState);

			if (CGameObject3D::NONE == *pCurHitState)
				return true;
			else
				return false;
		});

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsRangeInPlayer = { nullptr };
			pBlackBoard->Get_Type(L"isRangeInPlayer", pIsRangeInPlayer);
			if (nullptr == pIsRangeInPlayer)
				return false;

			return *pIsRangeInPlayer;
		});

	return S_OK;
}

HRESULT CAction_Attack::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CFollow* pFollow = dynamic_cast<CFollow*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Follow", m_pOwner));
	if (nullptr == pFollow)
		return E_FAIL;

	CAttack* pAttack = dynamic_cast<CAttack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Attack", m_pOwner));
	if (nullptr == pAttack)
		return E_FAIL;

	pFollow->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsAbleAttack = { nullptr };

			pBlackBoard->Get_Type(L"isAbleAttack", pIsAbleAttack);
			if (nullptr == pIsAbleAttack)
				return false;

			return !(*pIsAbleAttack);
		});

	pFollow->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			if (true == m_pModel->isLoopAnimation() ||
				true == m_pModel->isFinishedAnimation())
				return true;

			return false;
		});

	pFollow->Bind_AnimationTag("Walk_F");
	pFollow->Bind_Move_Speed(0.9f);
	pFollow->Bind_Turn_Speed(5.f);

	if (FAILED(Assemble_Behavior(L"Tsk_Follow", pFollow)))
		return E_FAIL;

	if (FAILED(Assemble_Behavior(L"Tsk_Attack", pAttack)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CAction_Attack* CAction_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAction_Attack* pInstance = new CAction_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAction_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAction_Attack* CAction_Attack::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CAction_Attack* pInstance = new CAction_Attack(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAction_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAction_Attack::Free()
{
	if(true == m_isCloned)
		Safe_Release(m_pModel);

	CSelector::Free();
}
