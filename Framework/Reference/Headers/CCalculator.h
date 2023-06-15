#pragma once

#include "CBase.h"

BEGIN(Engine)

class CCalculator final : public CBase
{
	DECLARE_SINGLETON(CCalculator)

private:
	explicit CCalculator();
	virtual ~CCalculator() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	_vector Picking_On_Triangle(const POINT& ptMouse, class CVIBuffer* pBuffer, class CTransform* pTransform);
	_vector Picking_On_Triangle(const POINT& ptMouse, class CModel* pModel, class CTransform* pTransform);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END