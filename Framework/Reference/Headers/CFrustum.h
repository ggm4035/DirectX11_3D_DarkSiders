#pragma once

#include "CBase.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)

private:
	explicit CFrustum() = default;
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Tick();

public:
	void Transform_To_LocalSpace(_fmatrix WorldMatrix);
	_bool isIn_WorldSpace(_fvector vWorldPosition, _float fRange);
	_bool isIn_LocalSpace(_fvector vLocalPosition, _float fRange);

private:
	_float3 m_vOriginal_Points[8];
	_float4 m_vWorldPlanes[6];
	_float4 m_vLocalPlanes[6];

public:
	virtual void Free() override;
};

END