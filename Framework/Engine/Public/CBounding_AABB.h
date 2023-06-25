#pragma once

#include "CBounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct tagBoundingAABBDesc : public BOUNDINGDESC
	{
		_float3 vExtents;
	}AABBDESC;

public:
	const BoundingBox* Get_BoundingSphere() const {
		return m_pAABB;
	}

private:
	explicit CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBounding_AABB(const CBounding_AABB& rhs);
	virtual ~CBounding_AABB() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render() override;
	virtual _bool Intersect(CCollider::TYPE eType, const CBounding* pBounding);

private:
	BoundingBox* m_pAABB_Original = { nullptr };
	BoundingBox* m_pAABB = { nullptr };

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding_AABB* Clone(void* pArg) override;
	virtual void Free() override;
};

END