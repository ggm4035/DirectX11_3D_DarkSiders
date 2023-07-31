#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CComposite
{
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	_bool is_Remove() const {
		return m_isRemove;
	}

public:
	virtual HRESULT Initialize_Prototype() = 0;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent * pOwner, void* pArg) = 0;
	virtual void Tick(const _double & TimeDelta);
	virtual void AfterFrustumTick(const _double & TimeDelta) {};
	virtual void Late_Tick(const _double & TimeDelta);
	virtual HRESULT Render() = 0;

protected:
	unordered_map<wstring, CGameObject*> m_Parts;

	_bool m_isRemove = { false };

protected:
	HRESULT Add_Parts(const _uint & iLevelIndex, const wstring & PrototypeTag, 
		const wstring & ObjectTag, CComponent * pOwner, void* pArg);
	CGameObject* Find_Parts(const wstring& wstrPartsTag);

public:
	virtual CGameObject* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;

protected:
	_bool m_isRender = { true };

public:
	void Togle_Render_Debug() {
		m_isRender = true == m_isRender ? false : true;
	}
#ifdef _DEBUG
#endif
};

END