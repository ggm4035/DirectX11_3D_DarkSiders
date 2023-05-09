#pragma once

#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CLevel();

public:
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	HRESULT Render();

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END