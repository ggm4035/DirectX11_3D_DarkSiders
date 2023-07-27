#pragma once

#include "CBehavior.h"

BEGIN(Engine)

class ENGINE_DLL CSelector : public CBehavior
{
protected:
	explicit CSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSelector(const CSelector& rhs);
	virtual ~CSelector() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta) override;

public:
	static CSelector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSelector* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END