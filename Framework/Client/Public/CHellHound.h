#pragma once

#include "Client_Defines.h"
#include "CMonster.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CHellHound final : public CMonster
{
private:
	explicit CHellHound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CHellHound(const CHellHound& rhs);
	virtual ~CHellHound() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render();

public:
	virtual void OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta) override;

private:
	CCollider* m_pColBody = { nullptr };
	CCollider* m_pColRange = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Make_AI();

public:
	static CHellHound* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CHellHound* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END