#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };

private:
	explicit CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, class CBounding* pBounding, void* pArg);

public:
	void Tick(_fmatrix WorldMatrix);
	HRESULT Render();

private:
	CBounding* m_pBounding = { nullptr };

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CCollider* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END