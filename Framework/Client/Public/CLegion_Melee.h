#pragma once

#include "Client_Defines.h"
#include "CMonster.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CLegion_Melee final : public CMonster
{
private:
	explicit CLegion_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLegion_Melee(const CLegion_Melee& rhs);
	virtual ~CLegion_Melee() = default;

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
	CCollider* m_pColAttack = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Make_AI();

public:
	static CLegion_Melee* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CLegion_Melee* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END