#pragma once

#include "CSequence.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CPattern_Patrol final : public CSequence
{
private:
	explicit CPattern_Patrol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPattern_Patrol(const CPattern_Patrol& rhs);
	virtual ~CPattern_Patrol() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	HRESULT Assemble_Childs();

public:
	static CPattern_Patrol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPattern_Patrol* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END