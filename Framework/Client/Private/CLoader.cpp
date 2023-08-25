
#include "CLoader.h"

#include "CGameInstance.h"
#include "CUI_Rect.h"
#include "CStatic_Object.h"
#include "CBreakAbleObject.h"
#include "CTerrain.h"
#include "CLava.h"
#include "CAoE.h"
#include "CSpawn.h"
#include "CSky.h"
#include "CCamera_Free.h"
#include "CGoblin.h"
#include "CGoblin_Armor.h"
#include "CHellHound.h"
#include "CLegion_Melee.h"
#include "CLegion_Champion.h"
#include "CSteamRoller.h"
#include "CHollowLord.h"
#include "CWeapon.h"
#include "CTrigger_Free.h"
#include "CSwordTrail.h"
#include "CAttackCore.h"
#include "CArmorCore.h"
#include "CHealthCore.h"

#include "CStone_Effect.h"
#include "CRoll_Effect.h"
#include "CBlood_Effect.h"
#include "CQuake_Effect.h"
#include "CSoul.h"

#include "CInven.h"
#include "CCurrency.h"

#include "CUI_Focus.h"
#include "CUI_Sprite.h"
#include "CUI_HpBar.h"
#include "CHUD.h"

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
	{
		LeaveCriticalSection(&m_Critical_Section);
		return E_FAIL;
	}

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

#pragma region Textures

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

	/* For. Texture_UI_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_Circle",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_Circle.png"))))
		return E_FAIL;

	/* For. Texture_UI_GenericCircle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_GenericCircle",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_GenericCircle.png"))))
		return E_FAIL;

	/* For. Texture_UI_UnitFrame_1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_UnitFrame_1",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_UnitFrame_1.png"))))
		return E_FAIL;

	/* For. Texture_UI_UnitFrame_2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_UnitFrame_2",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_UnitFrame_2.png"))))
		return E_FAIL;

	/* For. Texture_UI_UnitFrame_Hp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_UnitFrame_Hp",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_UnitFrameHp.png"))))
		return E_FAIL;

	/* For. Texture_UI_UnitFrame_HpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_UnitFrame_HpBar",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_UnitFrame_HpBar.png"))))
		return E_FAIL;

	/* For. Texture_UI_SkillFrame */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_SkillFrame",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_SkillFrame.png"))))
		return E_FAIL;

	/* For. Texture_UI_Ability_Dash */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_Ability_Dash",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_AbilityIcon_Dash.png"))))
		return E_FAIL;

	/* For. Texture_UI_Ability_Leap */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_Ability_Leap",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_AbilityIcon_Leap.png"))))
		return E_FAIL;

	/* For. Texture_UI_Ability_Wheel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_Ability_Wheel",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_AbilityIcon_Wheel.png"))))
		return E_FAIL;

	/* For. Texture_UI_HollowLord */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_HollowLord",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_HollowLord.png"))))
		return E_FAIL;

	/* For. Texture_UI_SteamRoller */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_SteamRoller",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_SteamRoller.png"))))
		return E_FAIL;

	/* For. Texture_UI_BossHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_BossHpBar",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_BossHpBar.png"))))
		return E_FAIL;

	/* For. Texture_Spawn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_Spawn",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/Spawn.png"))))
		return E_FAIL;

	/* For. Texture_UI_War */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_War",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_War.png"))))
		return E_FAIL;

	/* For. Texture_UI_CharBox */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_CharBox",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_CharBox.png"))))
		return E_FAIL;

	/* For. Texture_UI_AttackDS */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_AttackDS",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_AttackDS.png"))))
		return E_FAIL;

	/* For. Texture_UI_DeffenceDS */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_DeffenceDS",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_DeffenceDS.png"))))
		return E_FAIL;

	/* For. Texture_UI_HealthDS */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_HealthDS",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_HealthDS.png"))))
		return E_FAIL;

	/* For. Texture_UI_Focus */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_Focus",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_Focus.png"))))
		return E_FAIL;

	/* For. Texture_Screen_Focus */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_Screen_Focus",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/Screen_Focus.png"))))
		return E_FAIL;

	/* Cores */

	/* For. Texture_UI_AttackCore0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_AttackCore0",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_AttackCore0.png"))))
		return E_FAIL;

	/* For. Texture_UI_AttackCore1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_AttackCore1",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_AttackCore1.png"))))
		return E_FAIL;

	/* For. Texture_UI_AttackCore2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_AttackCore2",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_AttackCore2.png"))))
		return E_FAIL;

	/* For. Texture_UI_AttackCore3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_AttackCore3",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_AttackCore3.png"))))
		return E_FAIL;

	/* For. Texture_UI_ArmorCore0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_ArmorCore0",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_ArmorCore0.png"))))
		return E_FAIL;

	/* For. Texture_UI_ArmorCore1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_ArmorCore1",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_ArmorCore1.png"))))
		return E_FAIL;

	/* For. Texture_UI_ArmorCore2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_ArmorCore2",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_ArmorCore2.png"))))
		return E_FAIL;

	/* For. Texture_UI_ArmorCore3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_ArmorCore3",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_ArmorCore3.png"))))
		return E_FAIL;

	/* For. Texture_UI_HealthCore0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_HealthCore0",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_HealthCore0.png"))))
		return E_FAIL;

	/* For. Texture_UI_HealthCore1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_HealthCore1",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_HealthCore1.png"))))
		return E_FAIL;

	/* For. Texture_UI_HealthCore2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_HealthCore2",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_HealthCore2.png"))))
		return E_FAIL;

	/* For. Texture_UI_HealthCore3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_UI_HealthCore3",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_HealthCore3.png"))))
		return E_FAIL;

	/* ETC */

	/* For. Texture_SwordTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_SwordTrail",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/War_SwordTrail.png"))))
		return E_FAIL;

	/* For. Texture_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_Explosion",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Sprites/Explosion.png"))))
		return E_FAIL;

	/* For. Texture_RockChip */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_RockChip",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Sprites/RockChip.png"))))
		return E_FAIL;

	/* For. Texture_Impact */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_Impact",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Sprites/Impact.png"))))
		return E_FAIL;

	/* For. Texture_Soul */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_Soul",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/VFX/Soul.png"))))
		return E_FAIL;

	/* For. Texture_Currency_Soul */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_Currency_Soul",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_CurrencySouls.png"))))
		return E_FAIL;

	/* For. Texture_Inven */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_Inven",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_Inven.png"))))
		return E_FAIL;

	/* For. Texture_Dissolve */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_Dissolve",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/VFX/DissolveMask.png"))))
		return E_FAIL;

	/* For. Texture_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Texture_Particle",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/VFX/Particle.png"))))
		return E_FAIL;

#pragma endregion

	m_szLoading = TEXT("버퍼 로딩 중.");

	/* VIBuffer_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"VIBuffer_Trail",
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* VIBuffer_Sprite */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"VIBuffer_Sprite",
		CVIBuffer_Sprite::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_szLoading = TEXT("이펙트 로딩 중.");

	/* Stone_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Stone_Effect",
		CStone_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Quake_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Quake_Effect",
		CQuake_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Roll_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Roll_Effect",
		CRoll_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Blood_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Blood_Effect",
		CBlood_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Soul */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Soul",
		CSoul::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_szLoading = TEXT("UI 로딩 중.");

	/* For. UI_Focus */
	if (FAILED(m_pGameInstance->Add_Prototype(L"UI_Focus",
		CUI_Focus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. UI_HpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(L"UI_HpBar",
		CUI_HpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. HUD */
	if (FAILED(m_pGameInstance->Add_Prototype(L"HUD",
		CHUD::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. UI_Sprite */
	if (FAILED(m_pGameInstance->Add_Prototype(L"UI_Sprite",
		CUI_Sprite::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Models

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

	redupList.clear();
	for (auto& Data : FileData.vecBreakAbleData)
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

	/* SteamRoller Model */
	m_pGameInstance->ReadModel("../../Data/HollowLord.dat", FilePath, Data);

	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_HollowLord",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* SteamRoller Model */
	m_pGameInstance->ReadModel("../../Data/SteamRoller.dat", FilePath, Data);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_SteamRoller",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* SteamRoller Model */
	m_pGameInstance->ReadModel("../../Data/FallenDog.dat", FilePath, Data);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_FallenDog",
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

	/* Player Skill Models */

	/* Model_Rocks_Circle */
	PivotMatrix = XMMatrixRotationX(XMConvertToRadians(180.f));
	m_pGameInstance->ReadModel("../../ModelDatas/Skill/Rocks_Circle.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Rocks_Circle",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, XMMatrixIdentity()))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Model_Rocks_Cluster */
	m_pGameInstance->ReadModel("../../ModelDatas/Skill/Rocks_Cluster.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Rocks_Cluster",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, XMMatrixIdentity()))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Model_Rock_A */
	m_pGameInstance->ReadModel("../../ModelDatas/Skill/Rock_A.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Rock_A",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, XMMatrixIdentity()))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

	/* Model_Rock_B */
	m_pGameInstance->ReadModel("../../ModelDatas/Skill/Rock_B.dat", FilePath, Data);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Model_Rock_B",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, XMMatrixIdentity()))))
		return E_FAIL;
	Safe_Delete_BinaryData(Data);

#pragma endregion

	m_szLoading = TEXT("스테이터스 로딩 중.");

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Status_Health",
		CHealth::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Status_Attack",
		CAttack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Status_Deffence",
		CDeffence::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Inven",
		CInven::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Currency",
		CCurrency::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Attack_Core */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Attack_Core",
		CAttackCore::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Armor_Core */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Armor_Core",
		CArmorCore::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Health_Core */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Health_Core",
		CHealthCore::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_szLoading = TEXT("네비게이션 로딩 중.");

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Navigation",
		CNavigation::Create(m_pDevice, m_pContext, L"../../Data/Navigation.dat")))) // 네비게이션 저장한 경로가 있어야 쓸 수 있음
		return E_FAIL;

	m_szLoading = TEXT("객체 로딩 중.");

	/* For. SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype(L"SkyBox",
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Static_Object */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Static_Object",
		CStatic_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. BreakAble_Object */
	if (FAILED(m_pGameInstance->Add_Prototype(L"BreakAble_Object",
		CBreakAbleObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Monster_HollowLord */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Monster_HollowLord",
		CHollowLord::Create(m_pDevice, m_pContext))))
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

	/* For. AOE */
	if (FAILED(m_pGameInstance->Add_Prototype(L"AOE",
		CAoE::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Spawn */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Spawn",
		CSpawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Trigger_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Trigger_Free",
		CTrigger_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. SwordTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(L"SwordTrail",
		CSwordTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	list<CComponent*> data = m_pGameInstance->Get_All_Prototypes();
	m_szLoading = TEXT("로딩 완료.");

	for (auto& Data : FileData.vecModelData)
		Safe_Delete_BinaryData(Data.BinaryData);

	for (auto& Data : FileData.vecBreakAbleData)
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
