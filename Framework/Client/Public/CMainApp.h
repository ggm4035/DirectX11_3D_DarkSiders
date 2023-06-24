#pragma once

#include "Client_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)
	
class CMainApp final : public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(const _double& TimeDelta);
	HRESULT Render();

public:
	HRESULT Open_Level(LEVELID eLevelIndex);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CGameInstance* m_pGameInstance = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

	list<string> m_FilePathList;
	vector<MODEL_BINARYDATA> m_vecModelDatas;

private:
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Player();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END