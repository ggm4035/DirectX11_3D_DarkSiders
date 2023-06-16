#include "stdafx.h"
#include "CLoader.h"

#include "CGameInstance.h"
#include "CBackGround.h"
#include "CTerrain.h"
#include "CMonster.h"
#include "CCamera_Free.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
	Safe_AddRef(m_pGameInstance);
}

_uint WINAPI ThreadMain(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if(FAILED(pLoader->Loading()))
		return 0;

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	CoInitializeEx(nullptr, 0);

	EnterCriticalSection(&m_Critical_Section);

	HRESULT hResult = E_FAIL;

	switch (m_eNextLevelID)
	{
	case Client::LEVEL_LOGO:
		hResult = Load_Level_Logo();
		break;

	case Client::LEVEL_GAMEPLAY:
		hResult = Load_Level_GamePlay();
		break;
	}

	if (E_FAIL == hResult)
		return E_FAIL;

	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::Load_Level_Logo()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* 로고용 자원을 생성한다. */

	m_szLoading = TEXT("텍스쳐 로딩 중.");

	/* For. Prototype_Component_Texture_Test */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_Logo",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Default%d.dds", 2))))
		return E_FAIL;

	m_szLoading = TEXT("모델 로딩 중.");
	
	m_szLoading = TEXT("셰이더 로딩 중.");
		
	m_szLoading = TEXT("객체 로딩 중.");

	/* For. Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_BackGround",
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_szLoading = TEXT("로딩 완료.");

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_GamePlay()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	/* 게임 플레이용 자원을 생성한다. */

	m_szLoading = TEXT("텍스쳐 로딩 중.");

	/* For. Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Texture_Terrain",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Terrain/Tile%d.dds", 2))))
		return E_FAIL;

	m_szLoading = TEXT("모델 로딩 중.");

	/* For. Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_VIBuffer_Terrain",
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Terrain/Height.bmp"))))
		return E_FAIL;

	m_szLoading = TEXT("셰이더 로딩 중.");

	m_szLoading = TEXT("객체 로딩 중.");

	/* For. Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster",
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Terrain",
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Camera_Free",
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_szLoading = TEXT("로딩 완료.");

	m_bIsFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread); // 스레드 관련공부좀 해야할 듯...

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);
}
