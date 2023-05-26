#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CCamera final : public CComponent
{
private:
	CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	_matrix Get_CameraViewMatrix() { return XMLoadFloat4x4(&m_ViewMatrix); }
	_matrix Get_CameraProjMatrix() { return XMLoadFloat4x4(&m_ProjectionMatrix); }

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	void Set_View(_fmatrix _WorldMatrix);
	void Set_Projection(const _float& _fFov, const _float& _fAspect, const _float& _fNear, const _float& _fFar);
	void OnCamera() { m_bSwitch = true; }
	void OffCamera() { m_bSwitch = false; }

private:
	_float4x4 m_ViewMatrix;
	_float4x4 m_ProjectionMatrix;

private:
	_bool m_bSwitch = { false };

public:
	static CCamera* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END