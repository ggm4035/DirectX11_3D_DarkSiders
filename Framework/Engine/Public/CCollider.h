#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };
	enum STATE { STATE_NONE, STATE_ENTER, STATE_STAY, STATE_END };
	enum COLLIDERGROUP { COL_PLAYER, COL_PLAYER_ATK, 
		COL_ENEMY, COL_ENEMY_ATK, COL_ENEMY_MELEE_RANGE, COL_ENEMY_RANGE, 
		COL_END };

	typedef struct tagColliderDesc
	{
		COLLIDERGROUP eGroup;
		_float3 vOffset = { _float3() };
		_bool isEnable = { true };
	}COLLIDERDESC;

	typedef struct tagCollision
	{
		class CGameObject3D* pOwner = { nullptr };
		class CGameObject3D* pOther = { nullptr };
		CCollider* pMyCollider = { nullptr };
		CCollider* pOtherCollider = { nullptr };
		STATE eState = { STATE_NONE };
		_bool isCollision = { false };
	}COLLISION;

public:
	COLLIDERGROUP Get_Collider_Group() {
		return m_eColGroup;
	}
	_bool isEnable() const {
		return m_isEnable;
	}

private:
	explicit CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, class CBounding* pBounding, void* pArg);
	virtual HRESULT Render() override;

	void Tick(_fmatrix WorldMatrix);

public:
	_bool Intersect(const CCollider* pCollider);
	void On_Collision(const _double& TimeDelta);
	void Push_Collision_Message(CCollider* pMessage) {
		m_Qmessage.push(pMessage);
	}

private:
	class CBounding* m_pBounding = { nullptr };

	_float3 m_vOffset;

	TYPE m_eColliderType = { TYPE_END };
	COLLIDERGROUP m_eColGroup = { COL_END };

private:
	queue<CCollider*> m_Qmessage;
	unordered_map<CCollider*, COLLISION*> m_umapCollisions; /* 실시간으로 해당 콜라이더와 충돌중인 얘들을 저장 */
	
	_bool m_isEnable = { true };

private:
	void Find_Collision(CCollider* pCollider, COLLISION** pCollision);

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CCollider* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	virtual void Free() override;
};

END