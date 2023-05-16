#pragma once

#include "CBase.h"

BEGIN(Engine)

class CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_double		Get_Timer(const _tchar* pTimerTag);
	void		Set_Timer(const _tchar* pTimerTag);

public:
	HRESULT		Ready_Timer(const _tchar* pTimerTag);
	
private:
	class CTimer*		Find_Timer(const _tchar* pTimerTag);
	

private:
	unordered_map<const _tchar*, class CTimer*>		m_umapTimers;
	
public:
	virtual	void Free(void)override;
};

END
