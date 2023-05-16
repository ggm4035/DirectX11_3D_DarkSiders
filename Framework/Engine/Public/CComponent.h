#pragma once

#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Update_Component(_double TimeDelta) {}

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};


class ENGINE_DLL CComposite abstract : public CComponent
{
private:
	CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComposite(const CComposite& rhs);
	virtual ~CComposite() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update_Component(_double TimeDelta);

private:
	std::vector<CComponent*> m_vecComponents;

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END