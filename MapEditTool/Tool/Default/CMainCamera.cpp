#include "pch.h"
#include "CMainCamera.h"

#include "CGameInstance.h"

CMainCamera::CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMainCamera::CMainCamera(const CMainCamera& rhs)
	:CGameObject(rhs)
{
}

HRESULT CMainCamera::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainCamera::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CMainCamera::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
}

void CMainCamera::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
}

HRESULT CMainCamera::Render()
{
	if (FAILED(CGameObject::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainCamera::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Camera",
		L"Com_Camera", (CComponent**)&m_pCameraCom)))
		return E_FAIL;
	pGameInstance->Add_Camera(LEVEL_TOOLMAP, L"Camera_Main", m_pCameraCom);
	pGameInstance->On_Camera(LEVEL_TOOLMAP, L"Camera_Main");

	Safe_Release(pGameInstance);
	return S_OK;
}

CMainCamera* CMainCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMainCamera* pInstance = new CMainCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		AfxMessageBox(L"Failed to Created CMainCamera");
	}
	return pInstance;
}

CGameObject* CMainCamera::Clone(void* pArg)
{
	CMainCamera* pInstance = new CMainCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		AfxMessageBox(L"Failed to Cloned CMainCamera");
	}
	return pInstance;
}

void CMainCamera::Free()
{
	Safe_Release(m_pCameraCom);
	CGameObject::Free();
}
