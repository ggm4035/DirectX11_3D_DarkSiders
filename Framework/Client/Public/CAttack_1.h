#pragma once

#include "Client_Defines.h"
#include "CBehavior.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CAttack_1 final : public CBehavior
{
private:
	explicit CAttack_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAttack_1(const CAttack_1& rhs);
	virtual ~CAttack_1() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

private:
	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };
	_bool m_isFirst = { true };

public:
	static CAttack_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAttack_1* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END