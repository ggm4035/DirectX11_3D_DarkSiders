#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	explicit CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iXCount, _uint iZCount, _float fInterval);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

private:
	_uint m_iXCount = { 0 };
	_uint m_iZCount = { 0 };
	_float m_fInterval = { 0.f };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iXCount = 129, _uint iZCount = 129, _float fInterval = 1.f);
	virtual CVIBuffer_Terrain* Clone(void* pArg) override;
	virtual void Free() override;
};

END