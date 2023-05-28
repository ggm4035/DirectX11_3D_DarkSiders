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
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Add_Components() override;
	void Key_Input(_double TimeDelta);

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject3D* Clone(void* pArg) override;
	virtual void Free() override;
};

END