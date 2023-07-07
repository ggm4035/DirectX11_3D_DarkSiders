#include "CRoot.h"

#include "CBlackBoard.h"

CRoot::CRoot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CRoot::CRoot(const CRoot& rhs)
	: CBehavior(rhs)
{
}

HRESULT CRoot::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	m_pBlackBoard = CBlackBoard::Create();
	if (nullptr == m_pBlackBoard)
		return E_FAIL;

	return S_OK;
}

HRESULT CRoot::Tick(const _double& TimeDelta)
{
	HRESULT hr = 0;

	hr = m_iterCurBehavior->pBehavior->Tick(TimeDelta);

	switch (hr)
	{
	case BEHAVIOR_RUNNING:
		return S_OK;

	case BEHAVIOR_SUCCESS:
		m_iterCurBehavior = m_BehaviorList.begin();
		return S_OK;

	case BEHAVIOR_FAIL:
		m_iterCurBehavior = m_BehaviorList.begin();
		return S_OK;

	case BEHAVIOR_ERROR:
		return E_FAIL;
	}

	return E_FAIL;
}

CRoot* CRoot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoot* pInstance = new CRoot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRoot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRoot* CRoot::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CRoot* pInstance = new CRoot(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CRoot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoot::Free()
{
	CBehavior::Free();
}
