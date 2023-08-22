#pragma once

#include "CBehavior.h"
#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CMonoEffect final : public CBehavior
{
private:
	explicit CMonoEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMonoEffect(const CMonoEffect& rhs);
	virtual ~CMonoEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Bind_Effect(CGameObject3D* pEffect) {
		m_pEffect = pEffect;
		Safe_AddRef(m_pEffect);
	}

private:
	CGameObject3D* m_pEffect = { nullptr };

public:
	static CMonoEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMonoEffect* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END