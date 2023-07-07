#include "CBehavior.h"

#include "CBlackBoard.h"

CBehavior::CBehavior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_iterCurBehavior(m_BehaviorList.begin())
{
}

CBehavior::CBehavior(const CBehavior& rhs)
	: CComponent(rhs)
	, m_iterCurBehavior(m_BehaviorList.begin())
{
}

HRESULT CBehavior::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBehavior::AssembleBehavior(const wstring& BehaviorTag, CBehavior* _pBehavior)
{
	CBehavior* pBehavior = Find_Behavior(BehaviorTag);
	if (nullptr != pBehavior)
		return E_FAIL;

	_pBehavior->m_pParentBehavior = this;
	_pBehavior->m_pOwner = m_pOwner;
	_pBehavior->m_pBlackBoard = m_pBlackBoard;
	Safe_AddRef(m_pBlackBoard);

	m_BehaviorList.push_back({ BehaviorTag, _pBehavior });

	return S_OK;
}

CBehavior* CBehavior::Find_Behavior(const wstring& BehaviorTag)
{
	auto& iter = find_if(m_BehaviorList.begin(), m_BehaviorList.end(), [&](auto pBehavior)
		{
			if (BehaviorTag == pBehavior.wstrBehaviorTag)
				return true;
			else
				return false;
		});

	if (iter == m_BehaviorList.end())
		return nullptr;

	return iter->pBehavior;
}

void CBehavior::Free()
{
	for (auto& Pair : m_BehaviorList)
		Safe_Release(Pair.pBehavior);

	Safe_Release(m_pBlackBoard);

	CComponent::Free();
}