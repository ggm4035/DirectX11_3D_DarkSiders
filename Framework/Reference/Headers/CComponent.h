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

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

class ENGINE_DLL CComposite abstract : public CComponent
{
protected:
	CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComposite(const CComposite& rhs);
	virtual ~CComposite() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

protected:
	unordered_map<const _tchar*, CComponent*> m_Components;

protected:
	HRESULT Add_Component(_uint iNumLevel, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent * *ppOut, void* pArg = nullptr);

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END