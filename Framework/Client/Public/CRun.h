#pragma once

#include "Client_Defines.h"
#include "CBehavior.h"

class CRun final : public CBehavior
{
private:
	explicit CRun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRun(const CRun& rhs);
	virtual ~CRun() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(_fvector vDirection, const _double& TimeDelta);

public:
	static CRun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRun* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

