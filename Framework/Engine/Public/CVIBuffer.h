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
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(CComponent * pOwner, void* pArg) override;

public:
	virtual HRESULT Render();

protected:
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC m_BufferDesc = {};
	D3D11_SUBRESOURCE_DATA m_SubResourceData = {};

protected:
	_uint m_iNumVertices = { 0 };
	_uint m_iStride = { 0 }; /*정점 하나의 바이트 크기*/
	_uint m_iIndexStride = { 0 };
	_uint m_iNumIndices = { 0 };
	_uint m_iVertexBuffers = { 0 };

	DXGI_FORMAT m_eFormat;
	D3D11_PRIMITIVE_TOPOLOGY m_eTopology;

protected:
	HRESULT Create_Buffer(OUT ID3D11Buffer * *ppOut);

public:
	virtual CComponent* Clone(CComponent * pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END