#pragma once

#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	explicit CLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CLevel();

public:
	virtual HRESULT Initialize() { return S_OK; }
	virtual void Tick(_double TimeDelta) = 0;
	virtual HRESULT Render() = 0;

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END