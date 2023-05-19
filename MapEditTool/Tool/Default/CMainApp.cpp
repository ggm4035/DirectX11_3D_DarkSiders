#include "pch.h"
#include "CMainApp.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "CMyForm.h"

#include "CGameInstance.h"
#include "Tool_Defines.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CToolView* pToolView = dynamic_cast<CToolView*>(pMainFrame->m_MainSplitter.GetPane(0, 1));
	g_hWnd = pToolView->m_hWnd;

	GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WM_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(1, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(const _double TimeDelta)
{
	m_pGameInstance->Tick_Engine(TimeDelta);
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (FAILED(m_pGameInstance->Add_Prototype(0, L"Prototype_Component_Renderer",
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		AfxMessageBox(L"Failed to Created CMainApp");
	}
	
	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
