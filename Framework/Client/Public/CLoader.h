#pragma once

#include "Client_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	_bool Get_Finished() const { return m_bIsFinished; }
	wstring Get_LoadingText() const { return m_szLoading; }

public:
	HRESULT Initialize(LEVELID eNextLevelID);
	HRESULT Loading();

private:
	HANDLE m_hThread = { 0 };
	CRITICAL_SECTION m_Critical_Section;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	LEVELID m_eNextLevelID = { LEVEL_END };
	_bool m_bIsFinished = { false };
	wstring m_szLoading = { L"" };

private:
	HRESULT Load_Level_Logo();
	HRESULT Load_Level_GamePlay();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

END