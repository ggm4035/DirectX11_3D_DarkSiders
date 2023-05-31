#include "stdafx.h"
#include "CLevel_Loading.h"
#include "CLoader.h"
#include "CLevel_Logo.h"
#include "CLevel_GamePlay.h"
#include "CGameInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);

	return S_OK;
}

void CLevel_Loading::Tick(_double TimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (true == m_pLoader->Get_Finished())
		{
			CLevel* pLevel = { nullptr };

			switch (m_eNextLevelID)
			{
			case Client::LEVEL_LOGO:
				pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;

			case Client::LEVEL_GAMEPLAY:
				pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pLevel)
				return;

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			pGameInstance->Open_Level(m_eNextLevelID, pLevel);
			Safe_Release(pGameInstance);

			return;
		}
		else
			return;
	}

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
}

HRESULT CLevel_Loading::Render()
{
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created CLevel_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Loading::Free()
{
	CLevel::Free();

	Safe_Release(m_pLoader);
}
