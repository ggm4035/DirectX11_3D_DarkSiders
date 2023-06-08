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
	_vector Picking_On_Triangle(HWND hWnd, class CVIBuffer* pBuffer, class CTransform* pTransform);
	_float2 Get_ClientLeftTop(HWND hWnd);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END