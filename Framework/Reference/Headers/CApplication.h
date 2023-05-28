#pragma once

#include "CBase.h"

BEGIN(Engine)

class CApplication final : public CBase
{
	DECLARE_SINGLETON(CApplication)

private:
	explicit CApplication();
	virtual ~CApplication() = default;

public:
	static _uint m_iWinSizeX;
	static _uint m_iWinSizeY;

public:
	virtual void Free() override;
};

END