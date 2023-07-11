#include "CSequence.h"

CSequence::CSequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CSequence::CSequence(const CSequence& rhs)
	: CBehavior(rhs)
{
}

HRESULT CSequence::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSequence::Tick(const _double& TimeDelta)
{
	if (0 == m_BehaviorList.size())
		return E_FAIL;

	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	_int iResult = m_iterCurBehavior->pBehavior->Tick(TimeDelta);

	switch (iResult)
	{
	case BEHAVIOR_RUNNING:
		return BEHAVIOR_RUNNING;

	case BEHAVIOR_SUCCESS:
		++m_iterCurBehavior;

		if (m_iterCurBehavior == m_BehaviorList.end())
		{
			m_iterCurBehavior = m_BehaviorList.begin();
			return BEHAVIOR_SUCCESS;
		}
		else
			return Tick(TimeDelta);

	case BEHAVIOR_FAIL:
		m_iterCurBehavior = m_BehaviorList.begin();
		return BEHAVIOR_FAIL;

	case BEHAVIOR_ERROR:
		return BEHAVIOR_ERROR;
	}

	return E_FAIL;
}

CSequence* CSequence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSequence* pInstance = new CSequence(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSequence* CSequence::Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg)
{
	CSequence* pInstance = new CSequence(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CSequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence::Free()
{
	CBehavior::Free();
}
