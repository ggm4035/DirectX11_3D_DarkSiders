#pragma once

#include "CComponent.h"
#include "Client_Defines.h"

BEGIN(Client)

/* 인게임 재화 */
class CCurrency final : public CComponent
{
private:
	explicit CCurrency(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCurrency(const CCurrency& rhs);
	virtual ~CCurrency() = default;

public:
	const _uint& Get_Currency() const {
		return m_iCurrency;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;

public:
	void Add_Currency(const _uint& iValue) {
		m_iCurrency += iValue;
	}
	_bool Pay_Currency(const _uint& iValue);

private:
	_uint m_iCurrency = { 1000 };

public:
	static CCurrency* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCurrency* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END