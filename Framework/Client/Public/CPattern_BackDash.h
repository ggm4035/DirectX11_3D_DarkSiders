#pragma once

#include "CSequence.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

BEGIN(Client)

class CPattern_BackDash final : public CSequence
{
private:
	explicit CPattern_BackDash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPattern_BackDash(const CPattern_BackDash& rhs);
	virtual ~CPattern_BackDash() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta) override;

public:
	HRESULT Assemble_Childs();

private:
	CModel* m_pModel = { nullptr };
	CTransform* m_pTransform = { nullptr };

public:
	static CPattern_BackDash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPattern_BackDash* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END