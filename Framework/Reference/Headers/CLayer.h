#pragma once

#include "CBase.h"
#include "CObject_Manager.h"

BEGIN(Engine)

class CLayer final : public CBase
{
	friend CObject_Manager;

private:
	explicit CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Tick(const _double& TimeDelta);
	void Late_Tick(const _double& TimeDelta);

private:
	list<class CGameObject*> m_pGameObjects;

public:
	static CLayer* Create() { return new CLayer(); }
	virtual void Free() override;
};

END