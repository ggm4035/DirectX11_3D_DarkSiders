#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CDeffence final : public CComposite
{
public:
	typedef struct tagDeffenceDesc
	{
		_int iDeffence;
	}DEFFENCEDESC;

private:
	explicit CDeffence(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CDeffence(const CDeffence& rhs);
	virtual ~CDeffence() = default;

public:
	const _int& Get_Deffence() const{
		return m_iDeffence;
	}
	void Set_Deffence(const _int& iDeffence) {
		m_iDeffence = iDeffence;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) override;

private:
	_int m_iDeffence = { 0 };

public:
	static CDeffence* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CDeffence* Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg);
	virtual void Free() override;
};

END