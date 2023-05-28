#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
private:
	explicit CVIBuffer_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	virtual ~CVIBuffer_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Render() override;

public:
	static CVIBuffer_Rect* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CVIBuffer_Rect* Clone(void* pArg) override;
	virtual void Free() override;
};

END