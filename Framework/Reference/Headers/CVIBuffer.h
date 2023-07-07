#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	explicit CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	const vector<TRIANGLE>& Get_vecTriangle() const { return m_vecTriangle; }
	void Set_Triangle(const _uint& iIndex, const TRIANGLE& Triangle);
	void Set_Triangle_Dot(const _uint& iIndex, const _uint& iDot, const _float3& vValue);

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent * pOwner, void* pArg) override;
	virtual HRESULT Render();

public:
	HRESULT Begin(OUT D3D11_MAPPED_SUBRESOURCE& Out);
	void End();

protected:
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC m_BufferDesc = {};
	D3D11_SUBRESOURCE_DATA m_SubResourceData = {};
	D3D11_MAPPED_SUBRESOURCE m_MappedSubResource;

protected:
	_uint m_iNumVertices = { 0 };
	_uint m_iStride = { 0 }; /*정점 하나의 바이트 크기*/
	_uint m_iIndexStride = { 0 };
	_uint m_iNumIndices = { 0 };
	_uint m_iVertexBuffers = { 0 };

	_bool m_isCloned = { false };

	DXGI_FORMAT m_eFormat;
	D3D11_PRIMITIVE_TOPOLOGY m_eTopology;

	_float3* m_pVerticesPos = { nullptr };

	vector<TRIANGLE> m_vecTriangle;

protected:
	HRESULT Create_Buffer(OUT ID3D11Buffer * *ppOut);

public:
	virtual CComponent* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END