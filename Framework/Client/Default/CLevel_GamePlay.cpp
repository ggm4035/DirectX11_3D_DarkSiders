#include "stdafx.h"
#include "CLevel_GamePlay.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(CLevel::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);

	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다."));
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(CLevel::Render()))
		return E_FAIL;

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
