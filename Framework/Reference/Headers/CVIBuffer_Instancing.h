#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct tagInstanceDesc
	{
		vector<_float4x4> vecWorldMatrix;
		_uint iNumInstance;
	}INSTANCEDESC;

protected:
	explicit CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCEDESC* pInstanceDesc);
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Render() override;

public:
	HRESULT Begin_Instance(OUT VTXINSTANCE** ppSubResourceData);
	void End_Instance();

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	_uint m_iInstanceStride = { 0 };
	_uint m_iIndexCountPerInstance = { 0 };

protected: /* INSTANCE DESC */
	_uint m_iNumInstance = { 0 };
	_float4x4* m_pInstance_WorldMatrix = { nullptr };

public:
	virtual CComponent* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END