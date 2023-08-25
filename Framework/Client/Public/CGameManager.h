#pragma once

#include "CBase.h"
#include "Client_Defines.h"

BEGIN(Client)

class CGameManager final : public CBase
{
	DECLARE_SINGLETON(CGameManager)

private:
	explicit CGameManager() = default;
	virtual ~CGameManager() = default;

public:
	void Set_Cursor(class CCursor* pCursor);
	class CCursor* Get_Cursor() {
		return m_pCursor;
	}

	void SubBossDead() {
		m_isSubBossDead = true;
		m_isFirst = true;
	}
	void FinalBossDead() {
		m_isFinalBossDead = true;
		m_isFirst = true;
	}
	void Set_Slow(_bool& bData) {
		m_isSlow = bData;
	}
	_bool Get_Slow() {
		return m_isSlow;
	}

public:
	void Tick(const _double& TimeDelta);

private:
	class CCursor* m_pCursor = { nullptr };

	_bool m_isSubBossDead = { false };
	_bool m_isFinalBossDead = { false };
	_bool m_isFirst = { false };

private:
	_bool m_isSlow = { false };

public:
	virtual void Free() override;
};

END