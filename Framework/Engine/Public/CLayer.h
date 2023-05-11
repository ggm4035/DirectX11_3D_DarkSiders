#pragma once

#include "CBase.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);

private:
	list<class CGameObject*> m_pGameObjects;

public:
	static CLayer* Create() { return new CLayer(); }
	virtual void Free() override;
};

END