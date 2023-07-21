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
	virtual HRESULT Initialize_Prototype() { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	virtual HRESULT Render() { return S_OK; }

protected:
	_uint m_iLevelIndex = { 0 };
	CComponent* m_pOwner = { nullptr };
	wstring m_wstrTag = { L"" };

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual CComponent* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

class ENGINE_DLL CComposite abstract : public CComponent
{
protected:
	explicit CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CComposite(const CComposite& rhs);
	virtual ~CComposite() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent * pOwner, void* pArg) override;

public:
	CComponent* Get_Component(const wstring& wstrComponentTag);

protected:
	unordered_map<wstring, CComponent*> m_umapComponents;

protected:
	HRESULT Add_Component(const _uint& iLevelIndex, const wstring& PrototypeTag, const wstring& ComponentTag,
		CComponent **ppOut, CComponent * pOwner, void* pArg = nullptr);

public:
	virtual CComposite* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END