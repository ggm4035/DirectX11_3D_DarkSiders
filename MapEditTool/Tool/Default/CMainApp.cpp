#include "pch.h"
#include "CMainApp.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "CMyForm.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "Tool_Defines.h"

#include "CToolMap.h"

#include "CTerrain.h"
#include "CMainCamera.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	m_pMainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	m_pToolView = dynamic_cast<CToolView*>(m_pMainFrame->m_MainSplitter.GetPane(0, 1));
	g_hWnd = m_pToolView->m_hWnd;
	g_hInst = AfxGetInstanceHandle();

	TOOL->m_pToolView = m_pToolView;
	TOOL->m_pMainFrm = m_pMainFrame;

	GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInst = g_hInst;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WM_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_TOOLMAP, CToolMap::Create(m_pDevice, m_pContext))))
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

HRESULT CMainApp::Ready_Prototype_GameObject_For_Static()
{
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Terrain",
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Camera",
		CMainCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOLMAP, L"Prototype_Component_Renderer",
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOLMAP, L"Prototype_Component_VIBuffer_Terrain",
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOLMAP, L"Prototype_Component_VIBuffer_Rect",
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOLMAP, L"Prototype_Component_Shader_Vtxtex",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_Vtxtex.hlsl",
			VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOLMAP, L"Prototype_Component_Camera",
		CCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOLMAP, L"Prototype_Component_Texture_Test",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/De3.png"))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOLMAP, L"Prototype_Component_Transform",
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

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
	CToolInstance::DestroyInstance();

	CGameInstance::Release_Engine();
}
