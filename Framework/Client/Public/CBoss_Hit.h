#pragma once

#include "CSequence.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CBoss_Hit final : public CSequence
{
private:
	explicit CBoss_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss_Hit(const CBoss_Hit& rhs);
	virtual ~CBoss_Hit() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	HRESULT Assemble_Childs();

public:
	static CBoss_Hit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBoss_Hit* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END