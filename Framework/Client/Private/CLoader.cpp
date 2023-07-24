
#include "CLoader.h"

#include "CGameInstance.h"
#include "CUI_Rect.h"
#include "CStatic_Object.h"
#include "CTerrain.h"
#include "CLava.h"
#include "CSky.h"
#include "CCamera_Free.h"
#include "CGoblin.h"
#include "CGoblin_Armor.h"
#include "CHellHound.h"
#include "CLegion_Melee.h"
#include "CLegion_Champion.h"
#include "CSteamRoller.h"
#include "CWeapon.h"

#include "CUI_Overlay.h"
#include "CUI_HpBar.h"

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

	if (FAILED(pLoader->Loading()))
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
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, L"Texture_Logo",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_Title.png"))))
		return E_FAIL;

	m_szLoading = TEXT("모델 로딩 중.");

	m_szLoading = TEXT("셰이더 로딩 중.");

	m_szLoading = TEXT("객체 로딩 중.");

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

	/* For. Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_Terrain",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Terrain/Terrain%d.dds", 4))))
		return E_FAIL;

	/* For. Texture_SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_SkyBox",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/SkyBox/Sky_0.dds"))))
		return E_FAIL;

	/* For. Texture_NMTerrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_NMTerrain",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Terrain/Terrain2_nm.dds"))))
		return E_FAIL;

	/* For. Texture_Lava */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_Lava",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Lava/Lava_Lake_e.dds"))))
		return E_FAIL;

	/* For. Texture_UI_HpBar_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_HpBar_Monster",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_Health_Monster.png"))))
		return E_FAIL;

	m_szLoading = TEXT("UI 로딩 중.");

	/* For. UI_Overlay */
	if (FAILED(m_pGameInstance->Add_Prototype(L"UI_Overlay",
		CUI_Overlay::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. UI_HpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(L"UI_HpBar",
		CUI_HpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_szLoading = TEXT("모델 로딩 중.");

	FILEDATA FileData;
	m_pGameInstance->Load("../../Data/testmap.dat", FileData);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"VIBuffer_Terrain",
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Models */
	list<_tchar*> redupList;
	for (auto& Data : FileData.vecModelData)
	{
		auto iter = find_if(redupList.begin(), redupList.end(), [&](_tchar* pData) {
			if (0 == lstrcmp(pData, Data.BinaryData.szTag))
				return true;
			else
				return false;
			});

		if (iter == redupList.end())
		{
			redupList.push_back(Data.BinaryData.szTag);

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Data.BinaryData.szTag,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Data.BinaryData, XMMatrixIdentity()))))
				return E_FAIL;
		}
	}

	/* =====Monster Models=====  */
	string FilePath;
	MODEL_BINARYDATA Data;

	/* Goblin Model */
	m_pGameInstance->ReadModel("../../Data/SteamRoller.dat", FilePath, Data);

	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_SteamRoller",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Goblin Model */
	m_pGameInstance->ReadModel("../../Data/Goblin.dat", FilePath, Data);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Goblin",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Goblin_Armor Model */
	m_pGameInstance->ReadModel("../../Data/GoblinArmor.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Goblin_Armor",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* HellHound Model */
	m_pGameInstance->ReadModel("../../Data/HellHound.dat", FilePath, Data);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_HellHound",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Legion Model */
	m_pGameInstance->ReadModel("../../Data/Legion.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Legion",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Legion_Champion Model */
	m_pGameInstance->ReadModel("../../Data/LegionChampion.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Legion_Champion",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* ===== Monster Weapon Models ===== */

	/* Goblin_Sword Model */
	m_pGameInstance->ReadModel("../../ModelDatas/Weapon/Goblin_Sword.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Goblin_Sword",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Goblin_Quiver Model */
	m_pGameInstance->ReadModel("../../ModelDatas/Weapon/Goblin_Quiver.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Goblin_Quiver",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Goblin_Spear Model */
	m_pGameInstance->ReadModel("../../ModelDatas/Weapon/Goblin_Spear.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Goblin_Spear",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Legion_Weapon Model */
	m_pGameInstance->ReadModel("../../ModelDatas/Weapon/Legion_Weapon.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Legion_Weapon",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, XMMatrixIdentity()))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Legion_Spear Model */
	m_pGameInstance->ReadModel("../../ModelDatas/Weapon/Legion_Spear.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Legion_Spear",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, XMMatrixIdentity()))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Legion_Champion_Weapon_A Model */
	m_pGameInstance->ReadModel("../../ModelDatas/Weapon/Legion_Champion_Weapon_A.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Legion_Champion_Weapon_A",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, XMMatrixIdentity()))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Legion_Champion_Weapon_B Model */
	m_pGameInstance->ReadModel("../../ModelDatas/Weapon/Legion_Champion_Weapon_B.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Legion_Champion_Weapon_B",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, XMMatrixIdentity()))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	m_szLoading = TEXT("셰이더 로딩 중.");

	m_szLoading = TEXT("네비게이션 로딩 중.");

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Navigation",
		CNavigation::Create(m_pDevice, m_pContext, L"../../Data/Navigation.dat")))) // 네비게이션 저장한 경로가 있어야 쓸 수 있음
		return E_FAIL;

	m_szLoading = TEXT("객체 로딩 중.");

	/* For. SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype(L"SkyBox",
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Static Objects */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Static_Object",
		CStatic_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Monster_SteamRoller */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Monster_SteamRoller",
		CSteamRoller::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Monster_Goblin */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Monster_Goblin",
		CGoblin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Monster_Goblin_Armor */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Monster_Goblin_Armor",
		CGoblin_Armor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Monster_HellHound */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Monster_HellHound",
		CHellHound::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Monster_Legion_Melee */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Monster_Legion_Melee",
		CLegion_Melee::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Monster_Legion_Champion */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Monster_Legion_Champion",
		CLegion_Champion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Terrain",
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Lava */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Lava",
		CLava::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Camera_Free",
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	list<CComponent*> data = m_pGameInstance->Get_All_Prototypes();
	m_szLoading = TEXT("로딩 완료.");

	for (auto& Data : FileData.vecModelData)
		Safe_Delete_BinaryData(Data.BinaryData);

	for (auto& Data : FileData.vecMonsterData)
		Safe_Delete_BinaryData(Data.BinaryData);

	Safe_Delete_Array(FileData.pPositions);

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
