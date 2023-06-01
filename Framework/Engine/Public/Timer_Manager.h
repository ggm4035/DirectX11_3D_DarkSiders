#pragma once

#include "CBase.h"

BEGIN(Engine)

class CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	explicit CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_double		Get_Timer(wstring& pTimerTag);
	void		Set_Timer(wstring& pTimerTag);

public:
	HRESULT		Ready_Timer(wstring& pTimerTag);
	
private:
	class CTimer*		Find_Timer(wstring& pTimerTag);
	

private:
	unordered_map<wstring, class CTimer*>		m_umapTimers;
	
public:
	virtual	void Free(void)override;
};

END
