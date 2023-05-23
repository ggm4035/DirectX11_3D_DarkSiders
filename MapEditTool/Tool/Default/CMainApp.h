#pragma once

#include "Tool_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

class CMainApp : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(const _double TimeDelta);
	HRESULT Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

private:
	HRESULT Ready_Prototype_GameObject_For_Static();
	HRESULT Ready_Prototype_Component_For_Static();

public:
	static CMainApp* Create();
	virtual void Free() override;
};