#pragma once

#include "Client_Defines.h"
#include "CSequence.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CMonster_Patrol final : public CSequence
{
private:
	explicit CMonster_Patrol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMonster_Patrol(const CMonster_Patrol& rhs);
	virtual ~CMonster_Patrol() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	HRESULT Assemble_Childs();

public:
	static CMonster_Patrol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMonster_Patrol* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END