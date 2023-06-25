#pragma once

#include "CBounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct tagBoundingOBBDesc : public BOUNDINGDESC
	{
		_float3 vExtents;
		_float3 vRotation;
	}OBBDESC;

public:
	const BoundingOrientedBox* Get_BoundingSphere() const {
		return m_pOBB;
	}

private:
	explicit CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBounding_OBB(const CBounding_OBB& rhs);
	virtual ~CBounding_OBB() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render() override;
	virtual _bool Intersect(CCollider::TYPE eType, const CBounding* pBounding);

private:
	BoundingOrientedBox* m_pOBB_Original = { nullptr };
	BoundingOrientedBox* m_pOBB = { nullptr };

public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding_OBB* Clone(void* pArg) override;
	virtual void Free() override;
};

END