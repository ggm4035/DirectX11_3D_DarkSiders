#include "CSteamRoller.h"

#include "MonoBehavior_Defines.h"
#include "CBlackBoard.h"
#include "CGameManager.h"
#include "CGameInstance.h"
#include "CPlayer.h"
#include "CUI_Rect.h"

CSteamRoller::CSteamRoller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CSteamRoller::CSteamRoller(const CSteamRoller& rhs)
	: CMonster(rhs)
{
}

HRESULT CSteamRoller::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CMonster::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;

	XMStoreFloat4(&m_vResponPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pAttack->Set_Damage(1);
	m_pDeffence->Set_Deffence(0);
	m_pHealth->Set_Max_Hp(20);
	m_pHealth->Set_HP(20);

	return S_OK;
}

void CSteamRoller::Tick(const _double& TimeDelta)
{
	CMonster::Tick(TimeDelta);
	
	for (auto UI : m_pMonsterUI)
		UI->Tick(TimeDelta);
}

void CSteamRoller::AfterFrustumTick(const _double& TimeDelta)
{
	if (false == m_isSpawn)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		if (FAILED(Add_Colliders_To_Manager()))
		{
			MSG_BOX("Failed to Add Colliders To Manager");
			Safe_Release(pGameInstance);
			return;
		}

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
		if (true == m_isRender && FAILED(Add_Colliders_Debug_Render_Group(m_pRendererCom)))
			return;
#endif
	}

	Safe_Release(pGameInstance);
}

void CSteamRoller::Late_Tick(const _double& TimeDelta)
{
	if (false == m_isSpawn)
		return;

	for (auto UI : m_pMonsterUI)
		UI->Late_Tick(TimeDelta);

	On_Colisions(TimeDelta);
}

HRESULT CSteamRoller::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CSteamRoller::Dead_Motion(const _double& TimeDelta)
{
	CMonster::Dead_Motion(TimeDelta);

	static _float fTimeAcc = 0.f;
	fTimeAcc += TimeDelta;

	static _bool bFirst = { true };

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (fTimeAcc <= 0.5f)
	{
		if (FAILED(pGameInstance->Set_ChannelVolume(CSound_Manager::SOUND_BGM, 0.5f - fTimeAcc)))
			return;
	}

	if (true == bFirst)
	{
		pGameInstance->Play_Sound(L"mus_level02_boss_outro.ogg", CSound_Manager::SOUND_SUB_BGM, 0.5f);
		bFirst = false;
	}

	Safe_Release(pGameInstance);

	if (true == m_pModelCom->isFinishedAnimation())
	{
		CGameManager::GetInstance()->SubBossDead();
		m_isRemove = true;
	}
}

void CSteamRoller::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);

	if ((Collision.pMyCollider->Get_Tag() == L"Col_Attack" ||
		Collision.pMyCollider->Get_Tag() == L"Col_Attack_Roll") &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		Collision.pOther->Get_Damaged(m_pAttack);
	}
}

void CSteamRoller::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionStay(Collision, TimeDelta);

	if (Collision.pMyCollider->Get_Tag() == L"Col_Melee_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isAbleAttack = true;
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isRangeInPlayer = true;
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Attack_Roll" &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		static _float fTimeAcc = { 0.f };

		fTimeAcc += TimeDelta;

		if (0.3f <= fTimeAcc)
		{
			Collision.pOther->Get_Damaged(m_pAttack);
			fTimeAcc = 0.f;
		}
	}
}

void CSteamRoller::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (Collision.pMyCollider->Get_Tag() == L"Col_Melee_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isAbleAttack = false;
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isRangeInPlayer = false;
	}
}

HRESULT CSteamRoller::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_SteamRoller",
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	CBounding_Sphere::SPHEREDESC SphereDesc;
	CBounding_AABB::AABBDESC AABBDesc;
	CBounding_OBB::OBBDESC OBBDesc;

	/* Col_Body */
	AABBDesc.vExtents = _float3(2.f, 2.f, 2.f);
	AABBDesc.eGroup = CCollider::COL_ENEMY;
	AABBDesc.vOffset = _float3(0.f, 2.f, 0.f);
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Col_Body", &AABBDesc)))
		return E_FAIL;

	/* Col_Attack_Range */
	SphereDesc.fRadius = 7.5f;
	SphereDesc.eGroup = CCollider::COL_ENEMY_MELEE_RANGE;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);
	SphereDesc.vOffset = _float3(0.f, 0.f, 0.f);
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Melee_Range", &SphereDesc)))
		return E_FAIL;

	/* Col_Attack_Roll */
	SphereDesc.fRadius = 4.5f;
	SphereDesc.eGroup = CCollider::COL_ENEMY_ATK;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);
	SphereDesc.vOffset = _float3(0.f, 0.f, 0.f);
	SphereDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Attack_Roll", &SphereDesc)))
		return E_FAIL;

	/* Add_Col_Attack */
	OBBDesc.vExtents = _float3(1.5f, 1.f, 1.5f);
	OBBDesc.eGroup = CCollider::COL_ENEMY_ATK;
	OBBDesc.vOffset = _float3(0.f, 0.5f, 2.f);
	OBBDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_OBB", L"Col_Attack", &OBBDesc)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSteamRoller::Ready_UI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Rect::UIRECTDESC UIDesc;

	UIDesc.m_fX = _float(g_iWinSizeX >> 1);
	UIDesc.m_fY = 650;
	UIDesc.m_fSizeX = 500;
	UIDesc.m_fSizeY = 50;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_BossHpBar";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 2;
	m_pMonsterUI[0] = dynamic_cast<CUI_Rect*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"UI_Rect", L"HpFrame", nullptr, &UIDesc));
	if (nullptr == m_pMonsterUI[0])
		return E_FAIL;

	UIDesc.m_fX = _float(g_iWinSizeX >> 1);
	UIDesc.m_fY = 650;
	UIDesc.m_fSizeX = 460;
	UIDesc.m_fSizeY = 25;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_UnitFrame_HpBar";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 3;
	UIDesc.pHealth = m_pHealth;
	m_pMonsterUI[1] = dynamic_cast<CUI_Rect*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"UI_Rect", L"HpBar", nullptr, &UIDesc));
	if (nullptr == m_pMonsterUI[1])
		return E_FAIL;

	UIDesc.m_fX = 380;
	UIDesc.m_fY = 630;
	UIDesc.m_fSizeX = 100;
	UIDesc.m_fSizeY = 100;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_SteamRoller";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 2;
	m_pMonsterUI[2] = dynamic_cast<CUI_Rect*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"UI_Rect", L"UnitFrameSteamRoller", nullptr, &UIDesc));
	if (nullptr == m_pMonsterUI[2])
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

/* 보스임 */
HRESULT CSteamRoller::Make_AI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;
	CSequence* pSequence = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence", this));
	if (nullptr == pSequence)
		return E_FAIL;
	
	CAction* pAction_Rest = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Rest)
		return E_FAIL;
	CAction* pAction_Detect = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Detect)
		return E_FAIL;
	CAction_Hit* pSequence_Hit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Action_Hit", this));
	if (nullptr == pSequence_Hit)
		return E_FAIL;
	CPattern_Roll* pPattern_Roll = dynamic_cast<CPattern_Roll*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Pattern_Roll", this));
	if (nullptr == pPattern_Roll)
		return E_FAIL;
	CAction* pAction_Follow = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Follow)
		return E_FAIL;

	CLookAtTarget* pLookAtTarget = dynamic_cast<CLookAtTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_LookAtTarget", this));
	if (nullptr == pLookAtTarget)
		return E_FAIL;
	CBoss_Attack* pBoss_Attack = dynamic_cast<CBoss_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Boss_Attack", this));
	if (nullptr == pBoss_Attack)
		return E_FAIL;
	CFollow* pFollow = dynamic_cast<CFollow*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Follow", this));
	if (nullptr == pFollow)
		return E_FAIL;

	pAction_Rest->Bind_AnimationTag("Idle");
	pAction_Detect->Bind_AnimationTag("Enrage");
	pAction_Detect->Just_One_Time_Action();

	pSequence_Hit->Not_Look();
	pSequence_Hit->Not_Impact();
	pPattern_Roll->Set_CoolTime(15.f);
	pAction_Follow->Bind_AnimationTag("Walk_F");
	pBoss_Attack->Add_Attack_AnimTag("Attack_1");
	pBoss_Attack->Add_Attack_AnimTag("Attack_2");
	pBoss_Attack->Add_Attack_AnimTag("Attack_3");

	pAction_Rest->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsSpawn = { nullptr };
			pBlackBoard->Get_Type(L"isSpawn", pIsSpawn);
			if (nullptr == pIsSpawn)
				return false;

			return !(*pIsSpawn);
		});

	pAction_Follow->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				pBlackBoard->Get_Type(L"pHealth", pHealth);
				if (nullptr == pHealth)
					return false;

				if (CHealth::HIT_NONE == pHealth->Get_Current_HitState())
					return true;

				return false;
			});

	pAction_Follow->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsAbleAttack = { nullptr };
			pBlackBoard->Get_Type(L"isAbleAttack", pIsAbleAttack);
			if (nullptr == pIsAbleAttack)
				return false;

			return !(*pIsAbleAttack);
		});

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	/* 플레이어 발견 전부터 발견 직후 까지 */
	if (FAILED(pSelector->Assemble_Behavior(L"Action_Rest", pAction_Rest)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Action_Detect", pAction_Detect)))
		return E_FAIL;
	/* 플레이어 발견 이후 */
	if (FAILED(pSelector->Assemble_Behavior(L"Action_Hit", pSequence_Hit)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Pattern_Roll", pPattern_Roll)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Action_Follow", pAction_Follow)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Pattern_Attack", pSequence)))
		return E_FAIL;

	if (FAILED(pAction_Follow->Assemble_Behavior(L"Tsk_Follow", pFollow)))
		return E_FAIL;
	if (FAILED(pSequence->Assemble_Behavior(L"Tsk_Look", pLookAtTarget)))
		return E_FAIL;
	if (FAILED(pSequence->Assemble_Behavior(L"Boss_Attack", pBoss_Attack)))
		return E_FAIL;

	if (FAILED(pSequence_Hit->Assemble_Childs()))
		return E_FAIL;
	if (FAILED(pPattern_Roll->Assemble_Childs()))
		return E_FAIL;
	if (FAILED(pBoss_Attack->Assemble_Childs()))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CSteamRoller* CSteamRoller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSteamRoller* pInstance = new CSteamRoller(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSteamRoller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CSteamRoller* CSteamRoller::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CSteamRoller* pInstance = new CSteamRoller(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CSteamRoller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSteamRoller::Free()
{
	for (auto UI : m_pMonsterUI)
		Safe_Release(UI);

	CMonster::Free();
}
