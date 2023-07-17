#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CRandomLook final : public CBehavior
{
private:
	explicit CRandomLook(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRandomLook(const CRandomLook& rhs);
	virtual ~CRandomLook() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	static CRandomLook* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRandomLook* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END