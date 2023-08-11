#include "CMainApp.h"

#include "MonoBehavior_Defines.h"

#include "CGameInstance.h"
#include "CLevel_Loading.h"
#include "CPlayer.h"
#include "CWeapon.h"
#include "CCamera_Free.h"
#include "CUI_Rect.h"
#include "CCursor.h"
#include "CGameManager.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
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

	if (FAILED(Ready_UI_Objects()))
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

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	CUI_Rect::UIDESC Desc;
	Desc.m_fDepth = 0.f;
	Desc.m_fSizeX = 40.f;
	Desc.m_fSizeY = 40.f;
	Desc.m_fX = (_float)ptMouse.x;
	Desc.m_fY = (_float)ptMouse.y;

	m_pCursor = dynamic_cast<CCursor*>(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_STATIC, L"UI_Cursor", L"UI_Cursor", nullptr, &Desc));
	if (nullptr == m_pCursor)
		return E_FAIL;

	CGameManager::GetInstance()->Set_Cursor(m_pCursor);

	return S_OK;
}

void CMainApp::Tick(const _double& TimeDelta)
{
	m_TimeAcc += TimeDelta;

	m_pGameInstance->Tick_Engine(TimeDelta);

	m_pCursor->Tick(TimeDelta);
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

HRESULT CMainApp::Ready_UI_Objects()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(L"UI_Rect",
		CUI_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"UI_Rect",
		CUI_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"UI_Cursor",
		CCursor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
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

	/* Texture_Loding */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Texture_Loading",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_Loading.png"))))
		return E_FAIL;

	/* Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Texture_BackGround",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/emberBW.png"))))
		return E_FAIL;

	/* Texture_UI_Cursor */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Texture_UI_Cursor",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/UI/UI_Cursor.png"))))
		return E_FAIL;

	/* VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"VIBuffer_Rect",
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"VIBuffer_Cube",
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* VIBuffer_Sprite */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"VIBuffer_Sprite",
		CVIBuffer_Sprite::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Instance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Instance_Point",
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext))))
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

	/* Shader_PointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Shader_PointInstance",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl",
			VTXPOINTTEXINSTANCE_DECL::Elements, VTXPOINTTEXINSTANCE_DECL::iNumElements))))
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

	/* RandomSelector */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"RandomSelector",
		CRandomSelector::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* ==== Monster Behaviors ==== */

	/* Tsk_RandomLook */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_RandomLook",
		CRandomLook::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Wait */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Wait",
		CWait::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Hit */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Hit",
		CHit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Dead */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Dead",
		CDead::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Follow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Follow",
		CFollow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Pattern */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Action",
		CAction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_LookAtTarget */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_LookAtTarget",
		CLookAtTarget::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Jump */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Jump",
		CJump::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Tsk_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Tsk_Move",
		CMove::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* ==== Boss Actions ==== */

	/* Boss_Attack_DB*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Boss_Attack_DB",
		CBoss_Attack_DB::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Boss_Attack_LH*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Boss_Attack_LH",
		CBoss_Attack_LH::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Boss_Attack_RH*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Boss_Attack_RH",
		CBoss_Attack_RH::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Boss_Attack_LHS*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Boss_Attack_LHS",
		CBoss_Attack_LHS::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Boss_Attack_RHS*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Boss_Attack_RHS",
		CBoss_Attack_RHS::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Boss_Attacks*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Boss_Attacks",
		CBoss_Attacks::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* ==== Monster Actions ==== */

	/* Sequence_Patrol */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Sequence_Patrol",
		CPattern_Patrol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Action_Hit */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Action_Hit",
		CAction_Hit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Selector_Attack */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Pattern_Attack",
		CPattern_Attack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Pattern_Roll */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Pattern_Roll",
		CPattern_Roll::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Pattern_BackDash */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Pattern_BackDash",
		CPattern_BackDash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Pattern_Wheel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Pattern_Wheel",
		CPattern_Wheel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Boss_Attack */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Boss_Attack",
		CBoss_Attack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Player()
{
	string FilePath;
	MODEL_BINARYDATA Data;

	m_pGameInstance->ReadModel("../../Data/Warrior.dat", FilePath, Data);
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

	m_pGameInstance->ReadModel("../../ModelDatas/Weapon/Chaoseater.dat", FilePath, Data);

	m_vecModelDatas.push_back(Data);

	/* Weapon Model */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_PlayerWeapon",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Data, PivotMatrix))))
		return E_FAIL;

	/* For. PlayerAction */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerAction",
		CPlayerAction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Player_HitAction */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerHit",
		CPlayerHit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Player_MoveAction */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerMove",
		CPlayerMove::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Player_JumpAction */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerJump",
		CPlayerJump::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Player_AttackAction */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerAttack",
		CPlayerAttack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Player_Knockback */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerKnockback",
		CPlayerKnockback::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Player_WheelWind */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerWheelWind",
		CWheelWind::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Player_LeapAttack */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerLeapAttack",
		CLeapAttack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Parts()
{
	/* For. Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Weapon", CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Camera_Free", CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, L"Font_135", L"../../Resources/Fonts/135.spritefont")))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

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

	Safe_Release(m_pCursor);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CGameManager::GetInstance()->DestroyInstance();

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
