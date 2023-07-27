#pragma once

#include "CSelector.h"
#include "Client_Defines.h"

BEGIN(Client)

class CRandomSelector : public CSelector
{
protected:
	explicit CRandomSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRandomSelector(const CRandomSelector& rhs);
	virtual ~CRandomSelector() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta) override;

public:
	static CRandomSelector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRandomSelector* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END