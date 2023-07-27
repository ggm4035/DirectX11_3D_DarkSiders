#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CJump final : public CBehavior
{
private:
	explicit CJump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CJump(const CJump& rhs);
	virtual ~CJump() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Bind_Jump_Force(const _float& fForce) {
		m_fJumpForce = fForce;
	}

private:
	_float m_fJumpForce = { 0.f };

private:
	CTransform* m_pTransform = { nullptr };

public:
	static CJump* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CJump* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END