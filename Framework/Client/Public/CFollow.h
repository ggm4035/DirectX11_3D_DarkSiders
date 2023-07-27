#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CFollow final : public CBehavior
{
private:
	explicit CFollow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFollow(const CFollow& rhs);
	virtual ~CFollow() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Bind_Move_Speed(const _float& fMoveSpeed) {
		m_fMoveSpeed = fMoveSpeed;
	}
	void Bind_Turn_Speed(const _float& fTurnSpeed) {
		m_fTurnSpeed = fTurnSpeed;
	}
	void Set_Timer(const _float& fLimit) {
		m_fLimit = fLimit;
		m_isUseTimer = true;
	}

private:
	_float m_fMoveSpeed = { 1.f };
	_float m_fTurnSpeed = { 5.f };

	_bool m_isUseTimer = { false };
	_float m_fTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };

	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };

public:
	static CFollow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CFollow* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END