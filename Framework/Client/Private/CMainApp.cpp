#include "stdafx.h"
#include "CMainApp.h"

#include "MonoBehavior_Defines.h"

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
	srand((_uint)time(nullptr));

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

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Behaviors()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
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
	m_TimeAcc += TimeDelta;

	m_pGameInstance->Tick_Engine(TimeDelta);
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();

	++m_iRanderCount;

	if (1.f < m_TimeAcc)
	{
		m_wstrFPS = L"FPS : " + to_wstring(m_iRanderCount);

		m_TimeAcc = 0.f;
		m_iRanderCount = 0;
	}

	m_pGameInstance->Render_Font(L"Font_135", m_wstrFPS, _float2());

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

	/* VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"VIBuffer_Cube",
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Shader_VtxTex",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxTex.hlsl",
			VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Shader_VtxCube",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxCube.hlsl",
			VTXCUBE_DECL::Elements, VTXCUBE_DECL::iNumElements))))
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

HRESULT CMainApp::Ready_Behaviors()
{
	/* Root */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Root",
		CRoot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Selector */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Selector",
		CSelector::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Sequence */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Sequence",
		CSequence::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* ==== Behaviors ==== */

	/* Tsk_RandomLook */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_RandomLook",
		CRandomLook::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Run */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Run",
		CRun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Walk */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Walk",
		CWalk::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Wait */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Wait",
		CWait::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Spawn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Spawn",
		CSpawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Hit */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Hit",
		CHit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Attack_1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Attack_1",
		CAttack_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Attack_2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Attack_2",
		CAttack_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Attack_3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Attack_3",
		CAttack_3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* ==== Monster Actions ==== */

	/* Sequence_Patrol */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Sequence_Patrol",
		CMonster_Patrol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Selector_Attacks */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Selector_Attacks",
		CMonster_Attacks::Create(m_pDevice, m_pContext))))
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
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_PlayerWeapon",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;

	/* For. PlayerAction */
	if(FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerAction", 
		CPlayerAction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Player_MoveAction */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"MoveAction",
		CMoveAction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Player_JumpAction */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"JumpAction",
		CJumpAction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Player_AttackAction */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"AttackAction",
		CAttackAction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Parts()
{
	/* For. Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Weapon", CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	if(FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, L"Font_135", L"../../Resources/Fonts/135.spritefont")))
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
