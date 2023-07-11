#pragma once

#include "Client_Defines.h"
#include "CBehavior.h"
#include "CGameObject3D.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

BEGIN(Client)

class CHit final : public CBehavior
{
private:
	explicit CHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CHit(const CHit& rhs);
	virtual ~CHit() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

private:
	CModel* m_pModel = { nullptr };
	CTransform* m_pTransform = { nullptr };

public:
	static CHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CHit* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END