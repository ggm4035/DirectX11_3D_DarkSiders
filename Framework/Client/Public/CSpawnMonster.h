#pragma once

#include "CMonster.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CSelector;
END

BEGIN(Client)

class CSpawnMonster final : public CMonster
{
public:
	typedef struct tagSpawnDesc
	{
		wstring wstrModelTag = { L"" };
	}SPAWNDESC;

private:
	explicit CSpawnMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSpawnMonster(const CSpawnMonster& rhs);
	virtual ~CSpawnMonster() = default;

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
	CSelector* Make_Attack();

public:
	static CSpawnMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpawnMonster* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END