#pragma once

#include "Tool_Defines.h"
#include "CCamera.h"

BEGIN(Engine)
class CCamera;
class CTransform;
END

class CMainCamera final : public CCamera
{
private:
	CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMainCamera(const CMainCamera& rhs);
	virtual ~CMainCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bFix = { false };

private:
	virtual HRESULT Add_Components() override;
	void KeyInput(const _double TimeDelta);
	void Mouse_Move(_double TimeDelta);
	void Fix_Mouse();

public:
	static CMainCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMainCamera* Clone(void* pArg) override;
	virtual void Free() override;
};