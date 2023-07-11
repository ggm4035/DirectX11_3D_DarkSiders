#include "CBehavior.h"

#include "CDecoration.h"
#include "CBlackBoard.h"

CBehavior::CBehavior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_iterCurBehavior(m_BehaviorList.begin())
{
}

CBehavior::CBehavior(const CBehavior& rhs)
	: CComponent(rhs)
	, m_iterCurBehavior(m_BehaviorList.begin())
	, m_isCloned(true)
{
}

HRESULT CBehavior::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBehavior::Add_Decoration(function<_bool(class CBlackBoard*)> Func)
{
	if (nullptr == Func)
		return E_FAIL;

	CDecoration* pDecoration = CDecoration::Create(Func);
	if (nullptr == pDecoration)
		return E_FAIL;

	m_DecorationList.push_back(pDecoration);

	return S_OK;
}

_bool CBehavior::Check_Decorations()
{
	for (auto& Deco : m_DecorationList)
	{
		if (false == Deco->Is_Execute(m_pBlackBoard))
			return false;
	}

	return true;
}

HRESULT CBehavior::Assemble_Behavior(const wstring& BehaviorTag, CBehavior* _pBehavior)
{
	CBehavior* pBehavior = Find_Behavior(BehaviorTag);
	if (nullptr != pBehavior)
		return E_FAIL;

	_pBehavior->m_pParentBehavior = this;
	_pBehavior->m_pOwner = m_pOwner;
	_pBehavior->m_pBlackBoard = m_pBlackBoard;
	Safe_AddRef(m_pBlackBoard);

	BEHAVIOR Behavior;
	Behavior.wstrBehaviorTag = BehaviorTag;
	Behavior.pBehavior = _pBehavior;

	m_BehaviorList.push_back(Behavior);

	m_iterCurBehavior = m_BehaviorList.begin();

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
	for (auto& Desc : m_BehaviorList)
		Safe_Release(Desc.pBehavior);

	for (auto& Func : m_DecorationList)
		Safe_Release(Func);

	if(true == m_isCloned)
		Safe_Release(m_pBlackBoard);

	CComponent::Free();
}