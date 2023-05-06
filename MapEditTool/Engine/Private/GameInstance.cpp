#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device { CGraphic_Device::GetInstance() }
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CGameInstance::Initialize_Engine(const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	/* 디바이스 초기화. (그래픽, 사운드, 입력장치) */
	/* 레벨매니져 오브젝트 매니져, 컴포넌트 매니져들은 Reserve 한다.  */

	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(
		GraphicDesc.hWnd, 
		GraphicDesc.eWinMode, 
		GraphicDesc.iViewportSizeX, 
		GraphicDesc.iViewportSizeY, 
		ppDevice, ppContext)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_double TimeDelta)
{
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

void CGameInstance::Free()
{
	Safe_Release(m_pGraphic_Device);
}
