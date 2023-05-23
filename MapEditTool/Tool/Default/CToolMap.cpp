#include "pch.h"
#include "CToolMap.h"

#include "CGameInstance.h"

CToolMap::CToolMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CToolMap::Initialize()
{
	if (FAILED(CLevel::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_ToolMap(L"Layer_Static")))
		return E_FAIL;

	return S_OK;
}

void CToolMap::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);

}

HRESULT CToolMap::Render()
{
	if (FAILED(CLevel::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolMap::Ready_Layer_ToolMap(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOLMAP, L"Prototype_GameObject_Terrain", pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOLMAP, L"Prototype_GameObject_Camera", pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CToolMap* CToolMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolMap* pInstance = new CToolMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		AfxMessageBox(L"Failed to Created CToolMap");
	}
	return pInstance;
}

void CToolMap::Free()
{
	CLevel::Free();
}
