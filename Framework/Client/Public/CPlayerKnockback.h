#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CPlayerKnockback final : public CBehavior
{
private:
	explicit CPlayerKnockback(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayerKnockback(const CPlayerKnockback& rhs);
	virtual ~CPlayerKnockback() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	HRESULT AssembleBehaviors();

private:
	class CPlayer* m_pPlayer = { nullptr };
	class CAction* pAction_Loop = { nullptr };
	class CAction* pAction_Exit = { nullptr };

	_float3 m_vDirection;
	_bool m_isFirst = { true };

private:
	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };

public:
	static CPlayerKnockback* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPlayerKnockback* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END