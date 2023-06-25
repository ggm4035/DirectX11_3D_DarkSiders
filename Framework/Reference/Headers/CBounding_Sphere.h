#pragma once

#include "CBounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct tagBoundingSphereDesc : public BOUNDINGDESC
	{
		_float fRadius = { 0.f };
	}SPHEREDESC;

public:
	const BoundingSphere* Get_BoundingSphere() const {
		return m_pSphere;
	}
	void Set_Position(const _float3& vPosition) {
		m_pSphere->Center = vPosition;
	}

private:
	explicit CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBounding_Sphere(const CBounding_Sphere& rhs);
	virtual ~CBounding_Sphere() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render() override;
	virtual _bool Intersect(CCollider::TYPE eType, const CBounding* pBounding);

private:
	BoundingSphere* m_pSphere_Original = { nullptr };
	BoundingSphere* m_pSphere = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding_Sphere* Clone(void* pArg) override;
	virtual void Free() override;
};

END