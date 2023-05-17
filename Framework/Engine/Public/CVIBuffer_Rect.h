#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
private:
	CVIBuffer_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	~CVIBuffer_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Render() override;

public:
	static CVIBuffer_Rect* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END