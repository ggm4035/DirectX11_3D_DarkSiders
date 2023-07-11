#include "CSelector.h"

CSelector::CSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CSelector::CSelector(const CSelector& rhs)
	: CBehavior(rhs)
{
}

HRESULT CSelector::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSelector::Tick(const _double& TimeDelta)
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
		m_iterCurBehavior = m_BehaviorList.begin();
		return BEHAVIOR_SUCCESS;

	case BEHAVIOR_FAIL:
		++m_iterCurBehavior;

		if (m_iterCurBehavior == m_BehaviorList.end())
		{
			m_iterCurBehavior = m_BehaviorList.begin();
			return BEHAVIOR_FAIL;
		}
		else
			return Tick(TimeDelta);

	case BEHAVIOR_ERROR:
		return BEHAVIOR_ERROR;
	}

	return E_FAIL;
}

CSelector* CSelector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSelector* pInstance = new CSelector(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSelector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSelector* CSelector::Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg)
{
	CSelector* pInstance = new CSelector(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CSelector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelector::Free()
{
	CBehavior::Free();
}
