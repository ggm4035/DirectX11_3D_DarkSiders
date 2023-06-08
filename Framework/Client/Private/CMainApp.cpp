#include "stdafx.h"
#include "CMainApp.h"

#include "CGameInstance.h"
#include "CLevel_Loading.h"
#include "CPlayer.h"

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT Client::CMainApp::Initialize()
{
	GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInst = g_hInst;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WM_WIN;

	/* 엔진을 초기화한다. */
	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject_For_Static()))
		return E_FAIL;

	/* 초기 레벨을 생성하고 초기화 한다. */
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(const _double& TimeDelta)
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
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Renderer",
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect",
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationX(XMConvertToRadians(90.f));
	if(FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Player",
		CModel::Create(m_pDevice, m_pContext, 
			L"../Bin/Resources/Models/working/Environment/Void/VD_Basalt_Group_A/VD_Basalt_Group_A.fbx", PivotMatrix))))
		return E_FAIL;
	/*L"../Bin/Resources/Models/working/Environment/Hell/Structural/Ruins/Update/WallRubble_A/WallRubble_A.fbx"*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Shader_VtxTex",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxTex.hlsl", 
			VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Shader_VtxMesh",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxMesh.hlsl",
			VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Shader_VtxNorTex",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxNorTex.hlsl", 
			VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject_For_Static()
{
	/* For. Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player",
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelIndex)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelIndex));
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CMainApp");
		Safe_Release(pInstance);
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
