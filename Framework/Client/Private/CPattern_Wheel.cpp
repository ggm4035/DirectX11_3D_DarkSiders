#include "CPattern_Wheel.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CAction.h"
#include "CFollow.h"

CPattern_Wheel::CPattern_Wheel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSequence(pDevice, pContext)
{
}

CPattern_Wheel::CPattern_Wheel(const CPattern_Wheel& rhs)
	: CSequence(rhs)
{
}

HRESULT CPattern_Wheel::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CSequence::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	/* ÄðÅ¸ÀÓ */
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_float* pTimeAcc = { nullptr };
			pBlackBoard->Get_Type(L"fTimeAcc", pTimeAcc);
			if (nullptr == pTimeAcc)
				return false;

			_float fInterval = *pTimeAcc - m_fPreTimeAcc;
			if (m_fLimit > fInterval)
			{
				return false;
			}

			return true;
		});

	return S_OK; 
}

HRESULT CPattern_Wheel::Tick(const _double& TimeDelta)
{
	HRESULT hr = CSequence::Tick(TimeDelta);

	_float* pTimeAcc = { nullptr };
	m_pBlackBoard->Get_Type(L"fTimeAcc", pTimeAcc);
	if (nullptr == pTimeAcc)
		return E_FAIL;

	if (BEHAVIOR_SUCCESS == hr)
		m_fPreTimeAcc = *pTimeAcc;

	return hr;
}

HRESULT CPattern_Wheel::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CAction* pAction_Attack_3 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pAction_Attack_3)
		return E_FAIL;
	CAction* pEnter = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pEnter)
		return E_FAIL;
	CAction* pLoop = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pLoop)
		return E_FAIL;
	CAction* pExit = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pExit)
		return E_FAIL;

	CFollow* pFollow = dynamic_cast<CFollow*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Follow", m_pOwner));
	if (nullptr == pFollow)
		return E_FAIL;

	pAction_Attack_3->Bind_AnimationTag("Attack_3");
	pEnter->Bind_AnimationTag("Wheel_Start");
	pLoop->Bind_AnimationTag("Wheel_Loop");
	pLoop->NotLerp();
	pExit->Bind_AnimationTag("Wheel_Exit");

	pFollow->Bind_Move_Speed(1.2f);
	pFollow->Bind_Turn_Speed(2.f);
	pFollow->Set_Timer(10.f);

	if (FAILED(Assemble_Behavior(L"Action_Attack_3", pAction_Attack_3)))
		return E_FAIL;
	if (FAILED(Assemble_Behavior(L"Wheel_Enter", pEnter)))
		return E_FAIL;
	if (FAILED(Assemble_Behavior(L"Wheel_Loop", pLoop)))
		return E_FAIL;
	if (FAILED(Assemble_Behavior(L"Wheel_Exit", pExit)))
		return E_FAIL;

	if (pLoop->Assemble_Behavior(L"Tsk_Follow", pFollow))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CPattern_Wheel* CPattern_Wheel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPattern_Wheel* pInstance = new CPattern_Wheel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPattern_Wheel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPattern_Wheel* CPattern_Wheel::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPattern_Wheel* pInstance = new CPattern_Wheel(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CPattern_Wheel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPattern_Wheel::Free()
{
	CSequence::Free();
}
