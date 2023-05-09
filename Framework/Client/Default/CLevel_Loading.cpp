#include "stdafx.h"
#include "CLevel_Loading.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize()
{
	return S_OK;
}

void CLevel_Loading::Tick(_double TileDelta)
{
}

HRESULT CLevel_Loading::Render()
{
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Loading::Free()
{
	CLevel::Free();
}
