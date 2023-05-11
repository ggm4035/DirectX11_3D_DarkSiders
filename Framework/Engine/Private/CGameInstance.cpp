#include "..\Public\CGameInstance.h"
#include "CGraphic_Device.h"
#include "CLevel_Manager.h"
#include "CObject_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device { CGraphic_Device::GetInstance() }
	, m_pLevel_Manager{ CLevel_Manager::GetInstance() }
	, m_pObject_Manager{ CObject_Manager::GetInstance() }
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	/* ����̽� �ʱ�ȭ. (�׷���, ����, �Է���ġ) */

	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(
		GraphicDesc.hWnd, 
		GraphicDesc.eWinMode, 
		GraphicDesc.iViewportSizeX, 
		GraphicDesc.iViewportSizeY, 
		ppDevice, ppContext)))
		return E_FAIL;

	/* �����Ŵ��� ������Ʈ �Ŵ���, ������Ʈ �Ŵ������� Reserve �Ѵ�.  */
	// ���� ������Ʈ�� ���� ������ŭ ���� �迭 ����
	if (FAILED(m_pObject_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_double TimeDelta)
{
	// Tick �κ��� �Ƹ��� ���߿� �ٲ�� ���� ����.
	// ���� �� �ָ��� (nullptrüũ ����)
	if (nullptr == m_pLevel_Manager)
		return;

	m_pLevel_Manager->Tick(TimeDelta);

	m_pObject_Manager->Tick(1, TimeDelta);
}

void CGameInstance::Clear_LevelResources(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	// ������ ������ ���� ������Ʈ�� ���ҽ� ����.
	m_pObject_Manager->Clear_LevelResources(iLevelIndex);

	// ������ ������ ������Ʈ ����.

}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);	
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Open_Level(_uint iNumLevels, CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iNumLevels, pNewLevel);
}

HRESULT CGameInstance::Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObject(_uint iNumLayer, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iNumLayer, pPrototypeTag, pLayerTag, pArg);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();

	CObject_Manager::GetInstance()->DestroyInstance();

	CLevel_Manager::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	if(m_pLevel_Manager)
		Safe_Release(m_pLevel_Manager);
	if(m_pObject_Manager)
		Safe_Release(m_pObject_Manager);

	Safe_Release(m_pGraphic_Device);
}
