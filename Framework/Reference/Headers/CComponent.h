#pragma once

#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	explicit CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	void Set_Tag(wstring wstrTag) { m_wstrTag = wstrTag; }
	wstring Get_Tag() const { return m_wstrTag; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(CComponent* pOwner, void* pArg);

protected:
	CComponent* m_pOwner = { nullptr };
	wstring m_wstrTag = { L"" };

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual CComponent* Clone(CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

class ENGINE_DLL CComposite abstract : public CComponent
{
protected:
	explicit CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CComposite(const CComposite& rhs);
	virtual ~CComposite() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(CComponent * pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta);
	virtual void Late_Tick(const _double& TimeDelta);
	virtual HRESULT Render();

protected:
	unordered_map<wstring, CComponent*> m_Components;

protected:
	HRESULT Add_Component(const _uint& iNumLevel, const wstring& PrototypeTag, const wstring& ComponentTag, 
		CComponent **ppOut, CComponent * pOwner, void* pArg = nullptr);

public:
	virtual CComposite* Clone(CComponent * pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END