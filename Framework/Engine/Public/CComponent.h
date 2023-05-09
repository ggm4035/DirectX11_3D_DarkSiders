#pragma once

#include "CBase.h"

BEGIN(Engine)

class CComponent : public CBase
{
protected:
	CComponent();
	virtual ~CComponent();

public:
	void Update_Component(_double TileDelta);
	HRESULT Render();

private:
	// �׷��� ����̽�

public:
	virtual void Free() override;
};

class CComposite : public CComponent
{
private:
	CComposite();
	virtual ~CComposite();

private:
	// �׷��� ����̽�
	std::vector<CComponent*> m_vecComponents;

public:
	virtual void Free() override;
};

END