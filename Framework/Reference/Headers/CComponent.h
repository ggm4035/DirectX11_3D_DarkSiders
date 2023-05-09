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
	// 그래픽 디바이스

public:
	virtual void Free() override;
};

class CComposite : public CComponent
{
private:
	CComposite();
	virtual ~CComposite();

private:
	// 그래픽 디바이스
	std::vector<CComponent*> m_vecComponents;

public:
	virtual void Free() override;
};

END