#pragma once

#include "CGameObject3D.h"

BEGIN(Engine)

class ENGINE_DLL CTrigger abstract : public CGameObject3D
{
public:
	typedef struct tagTriggerDesc
	{
		_float4 vPosition;
		_float3 vExtents;
	}TRIGGERDESC;

protected:
	explicit CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) override;
	virtual void Tick(const _double & TimeDelta) override;
	virtual void Late_Tick(const _double & TimeDelta) override;
	virtual HRESULT Render() override { return S_OK; }

public:
	virtual void OnCollisionEnter(CCollider::COLLISION Collision, const _double & TimeDelta) = 0;
	virtual void OnCollisionStay(CCollider::COLLISION Collision, const _double & TimeDelta) = 0;
	virtual void OnCollisionExit(CCollider::COLLISION Collision, const _double & TimeDelta) = 0;

protected:
	TRIGGERDESC m_TriggerDesc;
	_bool m_isCall = { false };

protected:
	class CCollider* m_pColliderCom = { nullptr };

protected:
	virtual HRESULT Add_Components() = 0;

public:
	virtual CTrigger* Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END