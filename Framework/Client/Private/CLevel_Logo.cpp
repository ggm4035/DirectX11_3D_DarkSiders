#include "stdafx.h"
#include "CLevel_Logo.h"

#include "CGameInstance.h"
#include "CLevel_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(CLevel::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Logo()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;
	}
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(CLevel::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Prototype_Component_For_Logo()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, L"Prototype_GameObject_BackGround", pLayerTag)))
		return E_FAIL;

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
