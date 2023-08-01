#pragma once

#include "CCamera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	enum CAM_STATE { CAM_DEFAULT, CAM_SUBBOSS, CAM_FINALBOSS, CAM_END };

private:
	explicit CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	void Set_CamState(CAM_STATE eState) {
		m_eCamState = eState;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fRadian = { 0.f };
	_float3 m_vOffsets;
	_bool m_isSceneEnd = { false };

	CAM_STATE m_eCamState = { CAM_DEFAULT };

private:
	void FinalBoss(const _double& TimeDelta);
	void SubBoss(const _double& TimeDelta);
	void Default(const _double& TimeDelta);

private:
	virtual HRESULT Add_Components() override;
	void Key_Input(const _double& TimeDelta);

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCamera_Free* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END