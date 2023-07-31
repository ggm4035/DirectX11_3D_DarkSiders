#pragma once

#include "CMonster.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CSteamRoller final : public CMonster
{
private:
	explicit CSteamRoller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSteamRoller(const CSteamRoller& rhs);
	virtual ~CSteamRoller() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render();

public:
	virtual void Dead_Motion(const _double& TimeDelta) override;
	virtual void OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta) override;

private:
	_float4 m_vResponPosition;

private:
	virtual HRESULT Add_Components() override;
	HRESULT Make_AI();

public:
	static CSteamRoller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSteamRoller* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END