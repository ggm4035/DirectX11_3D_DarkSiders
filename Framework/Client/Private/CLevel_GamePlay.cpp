#include "stdafx.h"
#include "CLevel_GamePlay.h"

#include "CGameInstance.h"

#include "CPlayer.h"
#include "CTerrain.h"
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

	LightDesc.vPosition = _float4(500.f, 50.f, 500.f, 1.f);
	LightDesc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);
	LightDesc.fRange = 100.f;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.eType = CLight::TYPE_DIRECTIONAL;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(wstring pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FILEDATA FileData;
	if (FAILED(pGameInstance->Load("../../ModelDatas/testmap.dat", FileData)))
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
	PlayerDesc.WorldMatrix;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, L"Player",
		L"Player", pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(wstring pLayerTag)
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/*for (size_t i = 0; i < 20; i++)
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"Prototype_GameObject_Monster",
			L"Monster", pLayerTag)))
			return E_FAIL;
	}*/

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
	CLevel::Free();
}
