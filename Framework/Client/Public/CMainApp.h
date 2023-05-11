#pragma once

#include "Client_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
	
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	HRESULT Render();

public:
	HRESULT Open_Level(LEVELID eLevelIndex);

private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;

	CGameInstance*			m_pGameInstance;

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END