#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CComposite
{
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype() = 0;
	virtual HRESULT Initialize(CComponent * pOwner, void* pArg) = 0;
	virtual void Tick(const _double& TimeDelta) = 0;
	virtual void Late_Tick(const _double& TimeDelta) = 0;
	virtual HRESULT Render() = 0;

public:
	virtual CGameObject* Clone(CComponent * pOwner, void* pArg) = 0;
	virtual void Free() = 0;
};

END