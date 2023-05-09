#pragma once

#include "CComponent.h"

class CTransform final : CComponent
{
private:
	CTransform();
	virtual ~CTransform();

public:
	virtual void Free() override;
};

