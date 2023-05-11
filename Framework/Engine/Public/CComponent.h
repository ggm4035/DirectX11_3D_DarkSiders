#pragma once

#include "CBase.h"

BEGIN(Engine)

class CComponent : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CComponent();

public:
	virtual void Update_Component(_double TimeDelta) {}
	virtual HRESULT Render() { return S_OK; }

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

class CComposite : public CComponent
{
private:
	CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CComposite();

public:
	virtual void Update_Component(_double TimeDelta);
	virtual HRESULT Render();

private:
	std::vector<CComponent*> m_vecComponents;

public:
	virtual void Free() override;
};

END