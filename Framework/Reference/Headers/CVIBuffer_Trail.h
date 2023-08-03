#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
public:
	typedef struct tagTrailDesc
	{
		_uint iNumRect = { 0 };
		const _float3* pLowPosition = { nullptr };
		const _float3* pHightPosition = {nullptr};
	}TRAILDESC;

private:
	explicit CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual	HRESULT Initialize(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) override;
	virtual HRESULT Render();

	void Tick(const _float4x4& WorldMatrix);

private:
	_uint m_iNumRect = { 0 };
	const _float3* pLowPosition = { nullptr };
	const _float3* pHightPosition = { nullptr };

public:
	static CVIBuffer_Trail* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CVIBuffer_Trail* Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) override;
	virtual void Free() override;
};

END