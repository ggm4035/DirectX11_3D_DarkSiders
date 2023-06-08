#pragma once

#include "CBase.h"

class CConverter final : public CBase
{
private:
	explicit CConverter();
	virtual ~CConverter() = default;

public:
	HRESULT Initialize();
	void Tick();

public:
	static CConverter* Create();
	virtual void Free() override;
};

