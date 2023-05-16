#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer(const CVIBuffer& rhs);
	~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

protected:
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC m_BufferDesc = {};
	D3D11_SUBRESOURCE_DATA m_SubResourceData = {};
	_uint m_iNumVertices = { 0 };
	_uint m_iStride = { 0 }; /*정점 하나의 바이트 크기*/

protected:
	HRESULT Create_Buffer(OUT ID3D11Buffer * *ppOut);

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END