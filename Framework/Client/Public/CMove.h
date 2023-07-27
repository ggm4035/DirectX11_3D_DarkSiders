#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CMove final : public CBehavior
{
private:
	explicit CMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMove(const CMove& rhs);
	virtual ~CMove() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Bind_Move_Force(const _float& fForce) {
		m_fMoveForce = fForce;
	}
	void Bind_Direction(const _float3& vDirection) {
		m_vDirection = vDirection;
	}
	void Set_Timer(const _float& fTime) {
		m_fLimit = fTime;
		m_isUseTimer = true;
	}

private:
	_float m_fMoveForce = { 1.f };
	_float3 m_vDirection;

	_bool m_isUseTimer = { false };
	_float m_fLimit = { 0.f };
	_float m_fTimeAcc = { 0.f };

private:
	CTransform* m_pTransform = { nullptr };

public:
	static CMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMove* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END