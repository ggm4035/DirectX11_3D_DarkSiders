#pragma once

#include "CTrigger.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CTrigger_Free final : public CTrigger
{
private:
	explicit CTrigger_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTrigger_Free(const CTrigger_Free& rhs);
	virtual ~CTrigger_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override { return S_OK; }

public:
	virtual void OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta) override {}
	virtual void OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta) override {}

private:
	virtual HRESULT Add_Components() override;
	void Sub_Boss(CGameInstance* pGameInstance);
	void Final_Boss(CGameInstance* pGameInstance);

public:
	static CTrigger_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CTrigger_Free* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END