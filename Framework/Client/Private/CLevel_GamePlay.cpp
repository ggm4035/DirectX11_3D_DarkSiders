
#include "CLevel_GamePlay.h"

#include "CGameInstance.h"
#include "CWeapon.h"

#include "CPlayer.h"
#include "CTerrain.h"
#include "CGoblin.h"
#include "CStatic_Object.h"

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

	if (FAILED(Ready_Layer_Cameras(L"Layer_Cameras")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(const _double& TimeDelta)
{
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다."));
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
	LightDesc.vDirection = _float4(1.5f, -1.f, 0.8f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 0.75f, 0.75f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 0.75f, 0.75f, 1.f);
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

	m_PlayerWorldMatrix = FileData.WorldMatrix;
	m_vPlayerAngle = FileData.vAngle;

	m_vecMonsterDatas = FileData.vecMonsterData;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"SkyBox",
		L"SkyBox", pLayerTag)))
		return E_FAIL;
		
	Safe_Release(pGameInstance);

	for (auto& Data : FileData.vecModelData)
		Safe_Delete_BinaryData(Data.BinaryData);

	Safe_Delete_Array(FileData.pPositions);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Cameras(wstring pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Camera_Free",
		L"Camera_Free", pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(wstring pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer::PLAYERDESC PlayerDesc;

	PlayerDesc.WorldMatrix = m_PlayerWorldMatrix;
	PlayerDesc.vAngle = m_vPlayerAngle;
	PlayerDesc.SpeedPerSec = 4.f; // 걷기 = 1.5, 뛰기 3
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
	for (auto& Data : m_vecMonsterDatas)
		Safe_Delete_BinaryData(Data.BinaryData);
	CLevel::Free();
}
