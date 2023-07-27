#include "CRandomSelector.h"

CRandomSelector::CRandomSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSelector(pDevice, pContext)
{
}

CRandomSelector::CRandomSelector(const CRandomSelector& rhs)
	: CSelector(rhs)
{
}

HRESULT CRandomSelector::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CSelector::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}
/* 진짜 간단하게 짜봤음 나중에 시간 되면 가중치 둬서 만들어 보기 */
HRESULT CRandomSelector::Tick(const _double& TimeDelta)
{
	HRESULT hr = CSelector::Tick(TimeDelta);

	if (hr == BEHAVIOR_RUNNING)
		return BEHAVIOR_RUNNING;

	_uint iNumBehavior = m_BehaviorList.size();

	_uint iRandomNum = rand() % iNumBehavior;
	m_iterCurBehavior = m_BehaviorList.begin();
	for (_uint i = 0; i < iRandomNum; ++i)
		++m_iterCurBehavior;

	return hr;
}

CRandomSelector* CRandomSelector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRandomSelector* pInstance = new CRandomSelector(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRandomSelector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRandomSelector* CRandomSelector::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CRandomSelector* pInstance = new CRandomSelector(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CRandomSelector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandomSelector::Free()
{
	CSelector::Free();
}
