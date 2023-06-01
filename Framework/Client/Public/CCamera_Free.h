#pragma once

#include "Client_Defines.h"
#include "CCamera.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
private:
	explicit CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Add_Components() override;
	void Key_Input(const _double& TimeDelta);

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject3D* Clone(CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END