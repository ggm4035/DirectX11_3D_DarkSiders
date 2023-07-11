#pragma once

#include "Client_Defines.h"
#include "CBehavior.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CRun final : public CBehavior
{
private:
	explicit CRun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRun(const CRun& rhs);
	virtual ~CRun() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

private:
	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };
	_float m_fTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };

public:
	static CRun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRun* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END