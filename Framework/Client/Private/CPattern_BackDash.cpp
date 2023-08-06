#include "CPattern_BackDash.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CLookAtTarget.h"
#include "CAction.h"
#include "CWait.h"

CPattern_BackDash::CPattern_BackDash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSequence(pDevice, pContext)
{
}

CPattern_BackDash::CPattern_BackDash(const CPattern_BackDash& rhs)
	: CSequence(rhs)
{
}

HRESULT CPattern_BackDash::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CSequence::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	CGameObject3D* pGameObject = dynamic_cast<CGameObject3D*>(m_pOwner);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_pModel = dynamic_cast<CModel*>(pGameObject->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	m_pTransform = pGameObject->Get_Transform();
	if (nullptr == m_pTransform)
		return E_FAIL;
	Safe_AddRef(m_pTransform);
	
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

HRESULT CPattern_BackDash::Tick(const _double& TimeDelta)
{
	HRESULT hr = CSequence::Tick(TimeDelta);

	return hr;
}

HRESULT CPattern_BackDash::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Action */
	CAction* pAction_LookAt = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_LookAt)
		return E_FAIL;
	CAction* pAction_BackDash = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_BackDash)
		return E_FAIL;

	/* Movement */
	CLookAtTarget* pLookAtTarget = dynamic_cast<CLookAtTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_LookAtTarget", m_pOwner));
	if (nullptr == pLookAtTarget)
		return E_FAIL;
	CWait* pWait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Wait", m_pOwner));
	if (nullptr == pWait)
		return E_FAIL;
	
	pAction_LookAt->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CGameObject3D* pTarget = { nullptr };
			pBlackBoard->Get_Type(L"pTarget", pTarget);
			if (nullptr == pTarget)
				return false;

			_vector vTargetPos = pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			_vector vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
			//cout << XMVectorGetX(XMVector3Length(vTargetPos - vPosition)) << endl;
			if (1.5f < XMVectorGetX(XMVector3Length(vTargetPos - vPosition)))
				return false;

			return true;
		});

	pAction_LookAt->Bind_AnimationTag("Walk");
	pAction_BackDash->Bind_AnimationTag("Dash_Back");

	pWait->Set_Timer(1.f);

	if (FAILED(Assemble_Behavior(L"Action_LookAt", pAction_LookAt)))
		return E_FAIL;
	if (FAILED(Assemble_Behavior(L"Action_BackDash", pAction_BackDash)))
		return E_FAIL;

	if (FAILED(pAction_LookAt->Assemble_Behavior(L"Tsk_LookAtTarget", pLookAtTarget)))
		return E_FAIL;
	if (FAILED(pAction_BackDash->Assemble_Behavior(L"Tsk_Wait", pWait)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CPattern_BackDash* CPattern_BackDash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPattern_BackDash* pInstance = new CPattern_BackDash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPattern_BackDash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPattern_BackDash* CPattern_BackDash::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPattern_BackDash* pInstance = new CPattern_BackDash(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CPattern_BackDash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPattern_BackDash::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pModel);
		Safe_Release(m_pTransform);
	}

	CSequence::Free();
}
