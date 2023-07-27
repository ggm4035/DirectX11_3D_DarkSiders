#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Client)

class CDead final : public CBehavior
{
private:
	explicit CDead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDead(const CDead& rhs);
	virtual ~CDead() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	static CDead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDead* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END