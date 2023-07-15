#include "stdafx.h"
#include "CHit.h"

#include "CBlackBoard.h"
#include "CGameObject3D.h"
#include "CModel.h"

CHit::CHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CHit::CHit(const CHit& rhs)
	: CBehavior(rhs)
{
}

HRESULT CHit::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

	return S_OK;
}

HRESULT CHit::Tick(const _double& TimeDelta)
{
	CGameObject3D* pTarget = { nullptr };
	CGameObject3D::HITSTATE* pCurState = { nullptr };
	CGameObject3D::HITSTATE* pPreState = { nullptr };

	m_pBlackBoard->Get_Type(L"eCurHitState", pCurState);
	m_pBlackBoard->Get_Type(L"ePreHitState", pPreState);
	m_pBlackBoard->Get_Type(L"pTarget", pTarget);

	switch (*pCurState)
	{
	case CGameObject3D::NONE:
		if (*pPreState != *pCurState)
		{
			m_pModel->Change_Animation("Idle");
			*pPreState = *pCurState;
		}
		return BEHAVIOR_FAIL;

	case CGameObject3D::HIT:
		m_pTransform->LookAt(pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_pModel->Change_Animation("Impact");
		*pPreState = *pCurState;
		*pCurState = CGameObject3D::HITTING;
		return BEHAVIOR_SUCCESS;

	case CGameObject3D::HITTING:
		if (true == m_pModel->isFinishedAnimation())
		{
			*pCurState = CGameObject3D::NONE;
			return BEHAVIOR_FAIL;
		}
		return BEHAVIOR_RUNNING;
	}

	return BEHAVIOR_FAIL;
}

CHit* CHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHit* pInstance = new CHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHit* CHit::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CHit* pInstance = new CHit(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CHit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHit::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
