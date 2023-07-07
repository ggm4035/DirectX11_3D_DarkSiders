#pragma once

/* Renderer같은 느낌으로*/

#include "Observer_Interfaces.h"
#include "CBase.h"

BEGIN(Engine)

class CCollider_Manager final : public CBase, IObserver_Animation
{
	DECLARE_SINGLETON(CCollider_Manager)

public:
	enum COLLIDERGROUP{COL_PLAYER, COL_PLAYERATK, COL_ENEMY, COL_ENEMYATK, COL_ENEMYRANGE, COL_END};
	typedef struct tagNotifyColliderDesc : public NOTIFYDESC
	{

	}NOTIFYCOLLIDERDESC;

private:
	explicit CCollider_Manager() = default;
	virtual ~CCollider_Manager() = default;

public:
	HRESULT Add_Collider(COLLIDERGROUP eGroupID, class CCollider* pCollider);
	void Tick();
	void Clear_ColliderList();

private:
	list<class CCollider*> m_ColliderList[COL_END];

private:
	virtual void Update_Observer(NOTIFYDESC* pNotifyDesc) override;
	void Collision(COLLIDERGROUP SrcGroup, COLLIDERGROUP DestGroup);

public:
	virtual void Free() override;
};

END