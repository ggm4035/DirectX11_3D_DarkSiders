#pragma once

/* Renderer같은 느낌으로*/

#include "Observer_Interfaces.h"
#include "CCollider.h"

BEGIN(Engine)

class CCollider_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollider_Manager)

private:
	explicit CCollider_Manager() = default;
	virtual ~CCollider_Manager() = default;

public:
	HRESULT Add_Collider(class CCollider* pCollider);
	void Tick();
	void Clear_ColliderList();

private:
	list<CCollider*> m_ColliderList[CCollider::COL_END];

private:
	void Collision(CCollider::COLLIDERGROUP SrcGroup, CCollider::COLLIDERGROUP DestGroup);

public:
	virtual void Free() override;
};

END