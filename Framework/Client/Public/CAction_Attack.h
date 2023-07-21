#pragma once

#include "CSelector.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CAction_Attack final : public CSelector
{
private:
	explicit CAction_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAction_Attack(const CAction_Attack& rhs);
	virtual ~CAction_Attack() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	HRESULT Assemble_Childs();

private:
	CModel* m_pModel = { nullptr };

public:
	static CAction_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAction_Attack* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END