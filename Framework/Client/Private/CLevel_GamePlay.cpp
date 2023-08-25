#include "CLevel_GamePlay.h"

#include "CGameManager.h"
#include "CGameInstance.h"
#include "CWeapon.h"

#include "CPlayerAction.h"
#include "CPlayer.h"
#include "CTerrain.h"
#include "CGoblin.h"
#include "CStatic_Object.h"
#include "CBreakAbleObject.h"
#include "CTrigger_Free.h"
#include "CUI_Rect.h"
#include "CUI_HpBar.h"
#include "CHUD.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;
		
	if (FAILED(Ready_HUD()))
		return E_FAIL;
		
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Stop_AllSound()))
		return E_FAIL;

	if (FAILED(pGameInstance->Play_BGM(L"mus_level02_ambient.ogg", 0.6f)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_GamePlay::Tick(const _double& TimeDelta)
{
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다."));

	m_pHud->Tick(TimeDelta);
	m_pFadeIn->Tick(TimeDelta);

	m_pHud->Late_Tick(TimeDelta);
	m_pFadeIn->Late_Tick(TimeDelta);

	CGameManager::GetInstance()->Tick(TimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CLight::LIGHTDESC LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	LightDesc.vPosition = _float4(46.f, 92.f, 20.f, 1.f);//_float4(155, 40, 100.f, 1.f);//
	LightDesc.vDirection = _float4(1.5f, -1.f, 0.8f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 0.7f, 0.7f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 0.7f, 0.7f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);

	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(wstring pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FILEDATA FileData;
	if (FAILED(pGameInstance->Load("../../Data/testmap.dat", FileData)))
		return E_FAIL;

	CTerrain::TERRAINDESC TerrainDesc;

	TerrainDesc.wstrTextureTag = L"Texture_Terrain";
	TerrainDesc.iXCount = FileData.iXCount;
	TerrainDesc.iZCount = FileData.iZCount;
	TerrainDesc.pPositions = FileData.pPositions;

	/* Load Terrain */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Terrain",
		L"Terrain", pLayerTag, &TerrainDesc)))
		return E_FAIL;

	/* Make Lava */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Lava",
		L"Lava", pLayerTag)))
		return E_FAIL;

	/* Load Static Models */
	for (auto& Data : FileData.vecModelData)
	{
		CStatic_Object::STATICOBJECTDESC StaticObjectDesc;

		StaticObjectDesc.wstrModelTag = Data.BinaryData.szTag;
		StaticObjectDesc.WorldMatrix = Data.TransformMatrix;
		StaticObjectDesc.vAngle = Data.vAngle;

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Static_Object",
			Data.szObjectTag, pLayerTag, &StaticObjectDesc)))
			return E_FAIL;
	}

	/* Load BreakAble Models */
	for (auto& Data : FileData.vecBreakAbleData)
	{
		CBreakAbleObject::BREAKABLEDESC BreakAbleDesc;

		BreakAbleDesc.wstrModelTag = Data.BinaryData.szTag;
		BreakAbleDesc.WorldMatrix = Data.TransformMatrix;
		BreakAbleDesc.vAngle = Data.vAngle;
		BreakAbleDesc.vExtents = Data.vExtents;

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"BreakAble_Object",
			Data.szObjectTag, pLayerTag, &BreakAbleDesc)))
			return E_FAIL;
	}

	/* Load Trigger_Free */
	for (auto& Data : FileData.vecTriggerData)
	{
		CTrigger_Free::TRIGGERDESC Desc;

		Desc.vExtents = Data.vExtents;
		Desc.vPosition = Data.vPosition;

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Trigger_Free",
			Data.szTriggerTag, pLayerTag, &Desc)))
			return E_FAIL;
	}

	m_PlayerWorldMatrix = FileData.WorldMatrix;
	m_vPlayerAngle = FileData.vAngle;

	m_vecMonsterDatas = FileData.vecMonsterData;

	/* SkyBox*/
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"SkyBox", L"SkyBox", pLayerTag)))
		return E_FAIL;
		
	Safe_Release(pGameInstance);

	for (auto& Data : FileData.vecModelData)
		Safe_Delete_BinaryData(Data.BinaryData);

	for (auto& Data : FileData.vecBreakAbleData)
		Safe_Delete_BinaryData(Data.BinaryData);

	Safe_Delete_Array(FileData.pPositions);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(wstring pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer::PLAYERDESC PlayerDesc;

	PlayerDesc.WorldMatrix = m_PlayerWorldMatrix;
	PlayerDesc.vAngle = m_vPlayerAngle;
	PlayerDesc.SpeedPerSec = 6.f; // 걷기 = 1.5, 뛰기 3
	PlayerDesc.RotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Player",
		L"Player", pLayerTag, &PlayerDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(wstring pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	for (auto& Data : m_vecMonsterDatas)
	{
		CMonster::MONSTERDESC MonsterDesc;
		MonsterDesc.WorldMatrix = Data.TransformMatrix;
		MonsterDesc.vAngle = Data.vAngle;
		MonsterDesc.SpeedPerSec = 3.f;
		MonsterDesc.RotationPerSec = XMConvertToRadians(90.f);
		MonsterDesc.iNavigationIndex = Data.iNavigationIndex;
		MonsterDesc.isSpawn = false;
		
		wstring wstrObjTag = Data.BinaryData.szTag;

		if (wstring::npos != wstrObjTag.find(L"GoblinArmor"))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Monster_Goblin_Armor",
				L"Monster_Goblin_Armor", pLayerTag, &MonsterDesc)))
				return E_FAIL;
		}

		else if (wstring::npos != wstrObjTag.find(L"Goblin"))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Monster_Goblin",
				L"Monster_Goblin", pLayerTag, &MonsterDesc)))
				return E_FAIL;
		}

		if (wstring::npos != wstrObjTag.find(L"LegionChampion"))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Monster_Legion_Champion",
				L"Monster_Legion_Champion", pLayerTag, &MonsterDesc)))
				return E_FAIL;
		}
		else if (wstring::npos != wstrObjTag.find(L"Legion"))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Monster_Legion_Melee",
				L"Monster_Legion_Melee", pLayerTag, &MonsterDesc)))
				return E_FAIL;
		}
		
		if (wstring::npos != wstrObjTag.find(L"SteamRoller"))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Monster_SteamRoller",
				L"Monster_SteamRoller", pLayerTag, &MonsterDesc)))
				return E_FAIL;
		}
		
		if (wstring::npos != wstrObjTag.find(L"HollowLord"))
		{
			MonsterDesc.SpeedPerSec = 0.f;
			MonsterDesc.RotationPerSec = 0.f;

			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Monster_HollowLord",
				L"Monster_HollowLord", pLayerTag, &MonsterDesc)))
				return E_FAIL;
		}

		if (wstring::npos != wstrObjTag.find(L"HellHound"))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Monster_HellHound",
				L"Monster_HellHound", pLayerTag, &MonsterDesc)))
				return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_HUD()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pHud = dynamic_cast<CHUD*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"HUD", L"HUD", nullptr, nullptr));
	if (nullptr == m_pHud)
		return E_FAIL;

	CUI_Rect::UIRECTDESC UIDesc;

	UIDesc.m_fX = _float(g_iWinSizeX >> 1);
	UIDesc.m_fY = _float(g_iWinSizeY >> 1);
	UIDesc.m_fSizeX = (_float)g_iWinSizeX;
	UIDesc.m_fSizeY = (_float)g_iWinSizeY;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_Loading";
	UIDesc.iTextureLevelIndex = LEVEL_STATIC;
	UIDesc.iPassNum = 5;

	m_pFadeIn = dynamic_cast<CUI_Rect*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"UI_Rect", L"FadeIn", nullptr, &UIDesc));
	if (nullptr == m_pFadeIn)
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_GamePlay::Free()
{
	Safe_Release(m_pFadeIn);
	Safe_Release(m_pHud);

	for (auto& Data : m_vecMonsterDatas)
		Safe_Delete_BinaryData(Data.BinaryData);

	CGameManager::DestroyInstance();

	CLevel::Free();
}
