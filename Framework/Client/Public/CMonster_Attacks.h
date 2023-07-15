#pragma once

#include "Client_Defines.h"
#include "CSelector.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CMonster_Attacks final : public CSelector
{
private:
	explicit CMonster_Attacks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMonster_Attacks(const CMonster_Attacks& rhs);
	virtual ~CMonster_Attacks() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	HRESULT Assemble_Childs();

public:
	static CMonster_Attacks* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMonster_Attacks* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END