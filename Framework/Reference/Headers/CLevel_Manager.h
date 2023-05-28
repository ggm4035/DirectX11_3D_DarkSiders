#pragma once

#include "CBase.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)

private:
	explicit CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	void Tick(_double TimeDelta);

private:
	_uint m_iLevelIndex = { 0 };
	class CLevel* m_pCurrentLevel = { nullptr };

public:
	virtual void Free() override;
};

END