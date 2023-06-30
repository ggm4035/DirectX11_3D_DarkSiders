#include "stdafx.h"
#include "CMainApp.h"

#include "CGameInstance.h"
#include "CLevel_Loading.h"
#include "CPlayer.h"
#include "CWeapon.h"

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

	if (FAILED(Ready_Player()))
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

HRESULT CMainApp::Open_Level(LEVELID eLevelIndex)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelIndex));
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Renderer",
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	/* VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"VIBuffer_Rect",
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Shader_VtxTex",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxTex.hlsl",
			VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Shader_Mesh",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxMesh.hlsl",
			VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
		return E_FAIL;

	/* Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Shader_AnimMesh",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl",
			VTXANIMMESH_DECL::Elements, VTXANIMMESH_DECL::iNumElements))))
		return E_FAIL;

	/* Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Shader_VtxNorTex",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxNorTex.hlsl",
			VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
		return E_FAIL;

	/* For. Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Collider_Sphere",
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/* For. Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Collider_AABB",
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For. Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Collider_OBB",
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Player()
{
	string FilePath;
	MODEL_BINARYDATA Data;

	m_pGameInstance->ReadModel("../../Warrior.dat", FilePath, Data);
	m_vecModelDatas.push_back(Data);

	/* Player Model */
	_matrix PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(-XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Player",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;

	/* For. Player */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Player", CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_pGameInstance->ReadModel("../../Chaoseater.dat", FilePath, Data);

	m_vecModelDatas.push_back(Data);

	/* Weapon Model */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Weapon",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;

	/* For. Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Weapon", CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
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
	for (auto& Model : m_vecModelDatas)
		Safe_Delete_BinaryData(Model);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
