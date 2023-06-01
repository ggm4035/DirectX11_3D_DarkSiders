#pragma once

#include "CGameObject3D.h"
#include "CTransform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject3D
{
public:
	typedef struct tagCameraDesc
	{
		_float4 vEye, vAt, vUp;
		_float fFov, fAspect, fNear, fFar;
		CTransform::TRASNFORMDESC TransformDesc;
	}CAMERADESC;

protected:
	explicit CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void OnCamera() { m_bSwitch = true; }
	void OffCamera() { m_bSwitch = false; }

protected:
	class CPipeLine* m_pPipeLine = { nullptr };

protected:
	_float4 m_vEye, m_vAt, m_vUp;
	_float m_fFov, m_fAspect, m_fNear, m_fFar;
	_bool m_bSwitch = { false };

protected:
	virtual HRESULT Add_Components() override = 0;

public:
	virtual CGameObject3D* Clone(CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END