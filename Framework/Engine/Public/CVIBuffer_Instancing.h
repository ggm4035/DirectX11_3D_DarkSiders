#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct tagInstanceDesc
	{
		_uint iNumInstance;
	}INSTANCEDESC;

protected:
	explicit CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype() { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(vector<_float4x4>& vecMatrices);
	virtual HRESULT Render() override;

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	_uint m_iInstanceStride = { 0 };
	_uint m_iIndexCountPerInstance = { 0 };

protected: /* INSTANCE DESC */
	_uint m_iNumInstance = { 0 };

public:
	virtual CComponent* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END