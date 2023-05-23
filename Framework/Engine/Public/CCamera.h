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
	const _matrix* Get_CameraViewMatrix() const { return &m_ViewElements.ViewMatrix; }
	const _matrix* Get_CameraProjMatrix() const { return &m_ProjElements.ProjectionMatrix; }

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	void Tick(_double TimeDelta);

public:
	void OnCamera() { m_bSwitch = true; }
	void OffCamera() { m_bSwitch = false; }

private:
	VIEWELEM m_ViewElements;
	PROJELEM m_ProjElements;

private:
	_bool m_bSwitch = true;

public:
	static CCamera* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END