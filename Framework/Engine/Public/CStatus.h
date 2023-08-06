#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CStatus final : public CComposite
{
private:
	explicit CStatus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CStatus(const CStatus& rhs);
	virtual ~CStatus() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) override;

public:
	static CStatus* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CStatus* Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg);
	virtual void Free() override;
};

END