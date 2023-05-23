#include "CGameInstance.h"
#include "CGraphic_Device.h"
#include "CLevel_Manager.h"
#include "CObject_Manager.h"
#include "Timer_Manager.h"
#include "CComponent_Manager.h"
#include "CCamera_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device { CGraphic_Device::GetInstance() }
	, m_pLevel_Manager{ CLevel_Manager::GetInstance() }
	, m_pObject_Manager{ CObject_Manager::GetInstance() }
	, m_pTimer_Manager{ CTimer_Manager::GetInstance() }
	, m_pComponent_Manager{ CComponent_Manager::GetInstance() }
	, m_pCamera_Manager{ CCamera_Manager::GetInstance() }
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pCamera_Manager);
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	/* 디바이스 초기화. (그래픽, 사운드, 입력장치) */

	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(
		GraphicDesc.hWnd, 
		GraphicDesc.eWinMode, 
		GraphicDesc.iViewportSizeX, 
		GraphicDesc.iViewportSizeY, 
		ppDevice, ppContext)))
		return E_FAIL;

	/* 레벨매니져 오브젝트 매니져, 컴포넌트 매니져들은 Reserve 한다.  */
	// 게임 오브젝트의 레벨 개수만큼 동적 배열 생성
	if (FAILED(m_pObject_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pCamera_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_double TimeDelta)
{
	if (nullptr == m_pLevel_Manager)
		return;

	m_pObject_Manager->Tick(TimeDelta);
	m_pObject_Manager->Late_Tick(TimeDelta);

	m_pCamera_Manager->Tick(TimeDelta);

	m_pLevel_Manager->Tick(TimeDelta);
}

void CGameInstance::Clear_LevelResources(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return;

	// 삭제할 레벨의 게임 오브젝트의 리소스 삭제.
	m_pObject_Manager->Clear_LevelResources(iLevelIndex);

	// 삭제할 레벨의 컴포넌트 삭제.
	m_pComponent_Manager->Clear_LevelResources(iLevelIndex);

	// 삭제할 레벨의 카메라 삭제.
	m_pCamera_Manager->Clear_LevelResources(iLevelIndex);
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

_double CGameInstance::Get_Timer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0;

	return m_pTimer_Manager->Get_Timer(pTimerTag);
}

void CGameInstance::Set_Timer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;
	
	m_pTimer_Manager->Set_Timer(pTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Ready_Timer(pTimerTag);
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

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

const _matrix* CGameInstance::Get_Current_CameraViewMatrix() const
{
	if (nullptr == m_pCamera_Manager)
		return nullptr;

	return m_pCamera_Manager->Get_Current_CameraViewMatrix();
}

const _matrix* CGameInstance::Get_Current_CameraProjMatrix() const
{
	if (nullptr == m_pCamera_Manager)
		return nullptr;

	return m_pCamera_Manager->Get_Current_CameraProjMatrix();
}

HRESULT CGameInstance::Add_Camera(_uint iLevelIndex, const _tchar* pCameraTag, CCamera* pCamera)
{
	if (nullptr == m_pCamera_Manager)
		return E_FAIL;

	return m_pCamera_Manager->Add_Camera(iLevelIndex, pCameraTag, pCamera);
}

HRESULT CGameInstance::Remove_Camera(_uint iLevelIndex, const _tchar* pCameraTag)
{
	if (nullptr == m_pCamera_Manager)
		return E_FAIL;

	return m_pCamera_Manager->Remove_Camera(iLevelIndex, pCameraTag);
}

HRESULT CGameInstance::On_Camera(_uint iLevelIndex, const _tchar* pCameraTag)
{
	if (nullptr == m_pCamera_Manager)
		return E_FAIL;

	return m_pCamera_Manager->On_Camera(iLevelIndex, pCameraTag);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();

	CCamera_Manager::GetInstance()->DestroyInstance();

	CComponent_Manager::GetInstance()->DestroyInstance();

	CObject_Manager::GetInstance()->DestroyInstance();

	CLevel_Manager::GetInstance()->DestroyInstance();

	CTimer_Manager::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	if (m_pCamera_Manager)
		Safe_Release(m_pCamera_Manager);
	if (m_pComponent_Manager)
		Safe_Release(m_pComponent_Manager);
	if(m_pLevel_Manager)
		Safe_Release(m_pLevel_Manager);
	if(m_pObject_Manager)
		Safe_Release(m_pObject_Manager);
	if (m_pTimer_Manager)
		Safe_Release(m_pTimer_Manager);

	Safe_Release(m_pGraphic_Device);
}
