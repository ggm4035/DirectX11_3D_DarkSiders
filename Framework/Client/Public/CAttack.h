#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CAttack final : public CBehavior
{
private:
	explicit CAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAttack(const CAttack& rhs);
	virtual ~CAttack() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

private:
	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };

	_bool m_isFirst = { true };

public:
	static CAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAttack* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END