#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CComposite
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	virtual HRESULT Add_Components() = 0;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END