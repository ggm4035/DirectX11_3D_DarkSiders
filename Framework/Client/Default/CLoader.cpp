#include "stdafx.h"
#include "CLoader.h"

#include "CGameInstance.h"
#include "CBackGround.h"

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

	/* �ΰ�� �ڿ��� �����Ѵ�. */

	lstrcpy(m_szLoading, TEXT("�ؽ��� �ε� ��."));

	lstrcpy(m_szLoading, TEXT("�� �ε� ��."));

	lstrcpy(m_szLoading, TEXT("���̴� �ε� ��."));

	lstrcpy(m_szLoading, TEXT("��ü �ε� ��."));

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_BackGround",
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("�ε� �Ϸ�."));

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_GamePlay()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	/* ���� �÷��̿� �ڿ��� �����Ѵ�. */

	lstrcpy(m_szLoading, TEXT("�ؽ��� �ε� ��."));

	lstrcpy(m_szLoading, TEXT("�� �ε� ��."));

	lstrcpy(m_szLoading, TEXT("���̴� �ε� ��."));

	lstrcpy(m_szLoading, TEXT("��ü �ε� ��."));

	lstrcpy(m_szLoading, TEXT("�ε� �Ϸ�."));

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
	CloseHandle(m_hThread); // ������ ���ð����� �ؾ��� ��...

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);
}
