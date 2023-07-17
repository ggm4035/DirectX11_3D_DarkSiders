
#include "CAttack_2.h"

#include "CGameObject3D.h"
#include "CBlackBoard.h"
#include "CTransform.h"
#include "CModel.h"

CAttack_2::CAttack_2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CAttack_2::CAttack_2(const CAttack_2& rhs)
	: CBehavior(rhs)
{
}

HRESULT CAttack_2::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	CGameObject3D* pGameObject = dynamic_cast<CGameObject3D*>(m_pOwner);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_pTransform = pGameObject->Get_Transform();
	if (nullptr == m_pTransform)
		return E_FAIL;
	Safe_AddRef(m_pTransform);

	m_pModel = dynamic_cast<CModel*>(pGameObject->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* isAbleAttack = { nullptr };

			pBlackBoard->Get_Type<_bool*>(L"isAbleAttack", isAbleAttack);

			return *isAbleAttack;
		});

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CGameObject3D::HITSTATE* pCurState = { nullptr };
			pBlackBoard->Get_Type(L"eCurHitState", pCurState);

			if (CGameObject3D::NONE == *pCurState)
				return true;
			else
				return false;
		});

	return S_OK;
}

HRESULT CAttack_2::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
	{
		m_isFirst = true;
		return BEHAVIOR_FAIL;
	}

	CGameObject3D* pTarget = { nullptr };

	m_pBlackBoard->Get_Type(L"pTarget", pTarget);

	if (true == m_isFirst)
	{
		m_pTransform->LookAt(pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_pModel->Change_Animation("Attack_2");
		m_isFirst = false;
	}

	if (true == m_pModel->isFinishedAnimation())
	{
		m_isFirst = true;
		m_pModel->Change_Animation("Idle");
		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

CAttack_2* CAttack_2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttack_2* pInstance = new CAttack_2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAttack_2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAttack_2* CAttack_2::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CAttack_2* pInstance = new CAttack_2(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAttack_2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAttack_2::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
