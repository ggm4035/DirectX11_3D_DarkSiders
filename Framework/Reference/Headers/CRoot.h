#pragma once

#include "CBehavior.h"

BEGIN(Engine)

class ENGINE_DLL CRoot final : public CBehavior
{
private:
	explicit CRoot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRoot(const CRoot& rhs);
	virtual ~CRoot() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta) override;

public:
	static CRoot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRoot* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	virtual void Free() override;
};

END