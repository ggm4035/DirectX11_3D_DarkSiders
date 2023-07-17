#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CSpawn final : public CBehavior
{
private:
	explicit CSpawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSpawn(const CSpawn& rhs);
	virtual ~CSpawn() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

private:
	_bool m_isFirst = { true };
	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };

public:
	static CSpawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpawn* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END