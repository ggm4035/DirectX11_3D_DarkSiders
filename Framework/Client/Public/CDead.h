#pragma once

#include "CBehavior.h"
#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

BEGIN(Client)

class CDead final : public CBehavior
{
private:
	explicit CDead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDead(const CDead& rhs);
	virtual ~CDead() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

private:
	CModel* m_pModel = { nullptr };
	CTransform* m_pTransform = { nullptr };

	_bool m_isFirst = { true };

public:
	static CDead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDead* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END