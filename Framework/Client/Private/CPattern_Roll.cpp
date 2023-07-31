#include "CPattern_Roll.h"

#include "CBlackBoard.h"
#include "CGameInstance.h"

#include "CAction.h"
#include "CFollow.h"

CPattern_Roll::CPattern_Roll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSequence(pDevice, pContext)
{
}

CPattern_Roll::CPattern_Roll(const CPattern_Roll& rhs)
	: CSequence(rhs)
{
}

HRESULT CPattern_Roll::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CSequence::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_float* pTimeAcc = { nullptr };
			pBlackBoard->Get_Type(L"fTimeAcc", pTimeAcc);
			if (nullptr == pTimeAcc)
				return false;

			_float fInterval = *pTimeAcc - m_fPreTimeAcc;
			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	return S_OK;
}

HRESULT CPattern_Roll::Tick(const _double& TimeDelta)
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

HRESULT CPattern_Roll::Assemble_Childs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CAction* pStart = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pStart)
		return E_FAIL;
	CAction* pRunning = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pStart)
		return E_FAIL;
	CAction* pEnd = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", m_pOwner));
	if (nullptr == pStart)
		return E_FAIL;

	CFollow* pFollow = dynamic_cast<CFollow*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Follow", m_pOwner));
	if (nullptr == pFollow)
		return E_FAIL;

	pStart->Bind_AnimationTag("Roll_Start");
	pRunning->Bind_AnimationTag("Rolling");
	pRunning->NotLerp();
	pEnd->Bind_AnimationTag("Roll_Stop");

	pFollow->Bind_Move_Speed(2.f);
	pFollow->Bind_Turn_Speed(2.f);
	pFollow->Set_Timer(10.f);

	if (FAILED(Assemble_Behavior(L"Tsk_Roll_Start", pStart)))
		return E_FAIL;
	if (FAILED(Assemble_Behavior(L"Rolling", pRunning)))
		return E_FAIL;
	if (FAILED(Assemble_Behavior(L"Roll_Stop", pEnd)))
		return E_FAIL;

	if (pRunning->Assemble_Behavior(L"Tsk_Follow", pFollow))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CPattern_Roll* CPattern_Roll::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPattern_Roll* pInstance = new CPattern_Roll(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPattern_Roll");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPattern_Roll* CPattern_Roll::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPattern_Roll* pInstance = new CPattern_Roll(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CPattern_Roll");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPattern_Roll::Free()
{
	CSequence::Free();
}
