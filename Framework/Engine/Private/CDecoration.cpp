#include "CDecoration.h"

HRESULT CDecoration::Bind_Decoration(function<_bool(class CBlackBoard*)> Func)
{
	if (nullptr == Func)
		return E_FAIL;

	m_Function = Func;

	return S_OK;
}

_bool CDecoration::Is_Execute(CBlackBoard* pBlackBoard)
{
	if (nullptr == m_Function)
		return false;

	return m_Function(pBlackBoard);
}

CDecoration* CDecoration::Create(function<_bool(class CBlackBoard*)> Func)
{
	CDecoration* pInstance = new CDecoration();

	if (FAILED(pInstance->Initialize(Func)))
	{
		MSG_BOX("Failed To Create CDecoration");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecoration::Free()
{
}
