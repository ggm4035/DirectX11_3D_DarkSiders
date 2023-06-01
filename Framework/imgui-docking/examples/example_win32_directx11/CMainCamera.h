#pragma once

#include "IMGUI_Defines.h"
#include "CCamera.h"

BEGIN(Engine)
class CCamera;
class CTransform;
END

BEGIN(Imgui)

class CMainCamera final : public CCamera
{
private:
	CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMainCamera(const CMainCamera& rhs);
	virtual ~CMainCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bFix = { false };

private:
	virtual HRESULT Add_Components() override;
	void KeyInput(const _double& TimeDelta);

public:
	static CMainCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMainCamera* Clone(CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END
