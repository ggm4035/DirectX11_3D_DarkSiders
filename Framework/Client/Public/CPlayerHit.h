#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CHealth;
END

BEGIN(Client)

class CPlayerHit final : public CBehavior
{
private:
	explicit CPlayerHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayerHit(const CPlayerHit& rhs);
	virtual ~CPlayerHit() = default;

public:
	const _float& Get_HitTimeAcc() const {
		return m_fHitTimeAcc;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	HRESULT AssembleBehaviors();

private:
	class CPlayerKnockback* m_pKnockbackAction = { nullptr };

	_float m_fHitTimeAcc = { 0.f };

public:
	static CPlayerHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPlayerHit* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END