#include "..\Public\Timer_Manager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}

_double Engine::CTimer_Manager::Get_Timer(const wstring& TimerTag)
{
	CTimer*		pTimer = Find_Timer(TimerTag);
	if (nullptr == pTimer)
		return 0.0;

	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::Set_Timer(const wstring& TimerTag)
{
	CTimer*		pTimer = Find_Timer(TimerTag);
	if (nullptr == pTimer)
		return;

	pTimer->Update_Timer();
}

CTimer* Engine::CTimer_Manager::Find_Timer(const wstring& TimerTag)
{
	auto iter = m_umapTimers.find(TimerTag);

	if (iter == m_umapTimers.end())
		return nullptr;

	return iter->second;
}

HRESULT Engine::CTimer_Manager::Ready_Timer(const wstring& TimerTag)
{
	CTimer*	pTimer = Find_Timer(TimerTag);
	
	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_umapTimers.insert({ TimerTag, pTimer });

	return S_OK;
}

void Engine::CTimer_Manager::Free(void)
{
	for (auto& Pair : m_umapTimers)
	{
		Safe_Release(Pair.second);
	}
	m_umapTimers.clear();
}
