#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CPlayerHit final : public CBehavior
{
private:
	explicit CPlayerHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayerHit(const CPlayerHit& rhs);
	virtual ~CPlayerHit() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

private:
	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };

private:
	class CPlayer* m_pPlayer = { nullptr };

public:
	static CPlayerHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPlayerHit* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END