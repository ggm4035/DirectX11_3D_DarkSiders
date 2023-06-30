#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final : public CVIBuffer
{
private:
	explicit CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Cell(const CVIBuffer_Cell& rhs);
	virtual ~CVIBuffer_Cell() = default;

public:
	void Set_SpherePosition(const _uint & iIndex, const _float3 vPosition);

public:
	virtual HRESULT Initialize_Prototype(const _float3 * pPoints, class CBounding_Sphere* pPrototype);
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Render() override;
	HRESULT Render_Sphere();

public:
	_float3 Picking_Sphere(_fvector vOrigin, _fvector vDirection, OUT _int & iSphereIndex);

private:
	class CBounding_Sphere* m_pSphere[3] = { nullptr };

public:
	static CVIBuffer_Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3 * pPoints, class CBounding_Sphere* pPrototype = nullptr);
	virtual CVIBuffer_Cell* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END