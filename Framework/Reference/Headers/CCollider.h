#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };
	enum STATE { STATE_NONE, STATE_ENTER, STATE_STAY, STATE_END };

	typedef struct tagCollision
	{
		class CGameObject3D* pOther = { nullptr };
		class CTransform* pOtherTransform = { nullptr };
		_float4 vOtherPosition;
		CCollider* pMyCollider = { nullptr };
		CCollider* pOtherCollider = { nullptr };
	}COLLISION;

public:
	STATE Get_Current_State() const {
		return m_eCurrentState;
	}

	void Switch_On() {
		m_isEnable = true;
	}
	void Switch_Off() {
		m_isEnable = false;
	}

private:
	explicit CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, class CBounding* pBounding, void* pArg);
	void Tick(_fmatrix WorldMatrix, const _float3& vOffset = _float3());
	HRESULT Render();

public:
	_bool Intersect(const CCollider* pCollider);
	void On_Collision(class CGameObject3D* pOnwer, const _double& TimeDelta);
	void Push_Collision_Message(CCollider* pMessage) {
		m_Qmessage.push(pMessage);
	}

private:
	class CBounding* m_pBounding = { nullptr };
	_bool m_isEnable = { true };

	TYPE m_eColliderType = { TYPE_END };
	STATE m_eCurrentState = { STATE_END };

	queue<CCollider*> m_Qmessage;

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CCollider* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END