
#include "CLevel_Logo.h"

#include "CGameInstance.h"
#include "CLevel_Loading.h"
#include "CUI_Rect.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Play_BGM(L"mus_mainmenu.ogg", 0.5f)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Logo::Tick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
		{
			Safe_Release(pGameInstance);
			return;
		}
	}
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	Safe_Release(pGameInstance);
}

HRESULT CLevel_Logo::Render()
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(wstring pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Rect::UIRECTDESC UIDesc;
	UIDesc.m_fX = _float(g_iWinSizeX >> 1);
	UIDesc.m_fY = _float(g_iWinSizeY >> 1);
	UIDesc.m_fSizeX = (_float)g_iWinSizeX;
	UIDesc.m_fSizeY = (_float)g_iWinSizeY;
	UIDesc.wstrTextureTag = L"Texture_Logo";
	UIDesc.iTextureLevelIndex = LEVEL_LOGO;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, L"UI_Rect",
		L"BackGround", pLayerTag, &UIDesc)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	CLevel::Free();
}
