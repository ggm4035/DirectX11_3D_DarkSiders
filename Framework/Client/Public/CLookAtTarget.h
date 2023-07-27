#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CLookAtTarget final : public CBehavior
{
private:
	explicit CLookAtTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLookAtTarget(const CLookAtTarget& rhs);
	virtual ~CLookAtTarget() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Set_Timer(const _float& fLimit) {
		m_fLimit = fLimit;
		m_isUseTimer = true;
	}

private:
	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };

private:
	_bool m_isUseTimer = { false };
	_float m_fTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };

public:
	static CLookAtTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CLookAtTarget* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	virtual void Free() override;
};

END