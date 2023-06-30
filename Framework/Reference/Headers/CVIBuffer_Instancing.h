#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class CVIBuffer_Instancing abstract : public CVIBuffer
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
	virtual HRESULT Initialize_Prototype(const INSTANCEDESC* pInstanceDesc);
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	_uint m_iInstanceStride = { 0 };
	_uint m_iIndexCountPerInstance = { 0 };

	INSTANCEDESC m_InstanceDesc = { 0 };

public:
	virtual CComponent* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END