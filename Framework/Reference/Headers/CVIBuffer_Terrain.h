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
	virtual HRESULT Initialize_Prototype(const _uint& iXCount, const _uint & iZCount, const _float & fInterval);
	virtual HRESULT Initialize_Prototype(const wstring& wstrFilePath);
	virtual HRESULT Initialize(CComponent * pOwner, void* pArg) override;
	virtual HRESULT Render() override;
	HRESULT Load_Terrain(const _uint iXCount, const _uint iZCount, const _float3 * pPositions);
	HRESULT Load_Terrain();
	HRESULT Begin(OUT D3D11_MAPPED_SUBRESOURCE& Out);
	void End();

private:
	_uint m_iXCount = { 0 };
	_uint m_iZCount = { 0 };
	_float m_fInterval = { 0.f };

	D3D11_MAPPED_SUBRESOURCE m_MappedSubResource;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring wstrHeightMap);
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext,
		const _uint & iXCount = 257, const _uint & iZCount = 257, const _float & fInterval = 1.f);
	virtual CVIBuffer_Terrain* Clone(CComponent * pOwner, void* pArg) override;
	virtual void Free() override;
};

END