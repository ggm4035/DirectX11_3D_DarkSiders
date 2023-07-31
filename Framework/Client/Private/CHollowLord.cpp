#include "CHollowLord.h"

#include "MonoBehavior_Defines.h"
#include "CBlackBoard.h"
#include "CGameInstance.h"
#include "CPlayer.h"

#include "CUI_Rect.h"

CHollowLord::CHollowLord(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CHollowLord::CHollowLord(const CHollowLord& rhs)
	: CMonster(rhs)
{
}

HRESULT CHollowLord::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CMonster::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;

	XMStoreFloat4(&m_vResponPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_Status.iHP = 5;
	m_Status.iMaxHP = 5;
	m_isRangeInPlayer = true;



	return S_OK;
}

void CHollowLord::Tick(const _double& TimeDelta)
{
	if (CGameInstance::GetInstance()->Key_Down(DIK_T))
	{
		m_isSpawn = true;
	}
	if (CGameInstance::GetInstance()->Key_Down(DIK_2))
	{
		m_pModelCom->Change_Animation("Attack_1");
	}
	if (CGameInstance::GetInstance()->Key_Down(DIK_3))
	{
		m_pModelCom->Change_Animation("Attack_2");
	}
	if (CGameInstance::GetInstance()->Key_Down(DIK_4))
	{
		m_pModelCom->Change_Animation("Attack_3");
	}
	if (CGameInstance::GetInstance()->Key_Down(DIK_5))
	{
		m_pModelCom->Change_Animation("Attack_4");
	}
	if (CGameInstance::GetInstance()->Key_Down(DIK_6))
	{
		m_pModelCom->Change_Animation("Attack_5");
	}

	for (auto UI : m_pPlayerUI)
		UI->Tick(TimeDelta);

	for (auto UI : m_pPlayerUI)
		UI->Late_Tick(TimeDelta);

	CMonster::Tick(TimeDelta);
}

void CHollowLord::AfterFrustumTick(const _double& TimeDelta)
{
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

		if (FAILED(Add_Colliders_Debug_Render_Group(m_pRendererCom)))
			return;
#ifdef _DEBUG
#endif
	}

	Safe_Release(pGameInstance);
}

void CHollowLord::Late_Tick(const _double& TimeDelta)
{
	On_Colisions(TimeDelta);
}

HRESULT CHollowLord::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CHollowLord::Dead_Motion(const _double& TimeDelta)
{
	CMonster::Dead_Motion(TimeDelta);

	static _float fTimeAcc = 0.f;
	fTimeAcc += TimeDelta;

	if (fTimeAcc > 6.3f)
	{
		m_pTransformCom->Set_On_Navigation(false);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPosition.m128_f32[1] -= TimeDelta * 5.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

	if (true == m_pModelCom->isFinishedAnimation())
		m_isRemove = true;
}

void CHollowLord::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (wstring::npos != Collision.pMyCollider->Get_Tag().find(L"Attack") &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		Collision.pOther->Get_Damaged();
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Huge_Attack" &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		_float4 vPosition;
		XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		Collision.pOther->Get_Damaged_Knockback(vPosition);
	}
}

void CHollowLord::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
}

void CHollowLord::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
{
}

HRESULT CHollowLord::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_HollowLord",
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	CBounding_Sphere::SPHEREDESC SphereDesc;
	CBounding_AABB::AABBDESC AABBDesc;

	/* Col_Body */
	SphereDesc.fRadius = 15.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);
	SphereDesc.eGroup = CCollider::COL_BOSS;
	SphereDesc.vOffset = _float3(0.f, 0.f, 0.f);
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Body", &SphereDesc)))
		return E_FAIL;

	/* Col_Attack_LH */
	AABBDesc.vExtents = _float3(4.f, 2.f, 4.f);
	AABBDesc.eGroup = CCollider::COL_BOSS_ATK;
	AABBDesc.vOffset = _float3(-4.f, 7.f, 12.f);
	AABBDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Col_Attack_LH", &AABBDesc)))
		return E_FAIL;

	/* Col_Attack_RH */
	AABBDesc.vExtents = _float3(4.f, 2.f, 4.f);
	AABBDesc.eGroup = CCollider::COL_BOSS_ATK;
	AABBDesc.vOffset = _float3(4.f, 7.f, 12.f);
	AABBDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Col_Attack_RH", &AABBDesc)))
		return E_FAIL;

	/* Col_Attack_Area */
	AABBDesc.vExtents = _float3(15.f, 2.f, 5.f);
	AABBDesc.eGroup = CCollider::COL_BOSS_ATK;
	AABBDesc.vOffset = _float3(0.f, 7.f, 20.f);
	AABBDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Col_Attack_Area", &AABBDesc)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHollowLord::Ready_UI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Rect::UIRECTDESC UIDesc;

	UIDesc.m_fX = 250;
	UIDesc.m_fY = 110;
	UIDesc.m_fSizeX = 220;
	UIDesc.m_fSizeY = 40;
	UIDesc.m_fDepth = 0.01f;
	UIDesc.wstrTextureTag = L"Texture_UI_UnitFrame_Hp";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 2;
	m_pPlayerUI[0] = dynamic_cast<CUI_Rect*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"UI_Rect", L"UnitFrameHp", nullptr, &UIDesc));
	if (nullptr == m_pPlayerUI[0])
		return E_FAIL;

	UIDesc.m_fX = 100;
	UIDesc.m_fY = 100;
	UIDesc.m_fSizeX = 100;
	UIDesc.m_fSizeY = 100;
	UIDesc.m_fDepth = 0.01f;
	UIDesc.wstrTextureTag = L"Texture_UI_UnitFrame_2";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 2;
	m_pPlayerUI[1] = dynamic_cast<CUI_Rect*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"UI_Rect", L"UnitFrame2", nullptr, &UIDesc));
	if (nullptr == m_pPlayerUI[1])
		return E_FAIL;

	UIDesc.m_fX = 100;
	UIDesc.m_fY = 100;
	UIDesc.m_fSizeX = 100;
	UIDesc.m_fSizeY = 100;
	UIDesc.m_fDepth = 0.01f;
	UIDesc.wstrTextureTag = L"Texture_UI_UnitFrame_2";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 2;
	UIDesc.pMaxHp = &m_Status.iMaxHP;
	UIDesc.pHp = &m_Status.iHP;
	m_pPlayerUI[2] = dynamic_cast<CUI_Rect*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"UI_Rect", L"UnitFrame2", nullptr, &UIDesc));
	if (nullptr == m_pPlayerUI[2])
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

/* ∫∏Ω∫¿” */
HRESULT CHollowLord::Make_AI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;

	CAction* pAction_Rest = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Rest)
		return E_FAIL;
	CAction_Hit* pAction_Hit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Action_Hit", this));
	if (nullptr == pAction_Hit)
		return E_FAIL;
	CBoss_Attack* pBoss_Attack = dynamic_cast<CBoss_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Boss_Attack", this));
	if (nullptr == pBoss_Attack)
		return E_FAIL;

	pAction_Rest->Bind_AnimationTag("Idle");
	pAction_Hit->Not_Impact();
	pAction_Hit->Not_Look();
	pBoss_Attack->Set_CoolTime(1.7f);
	pBoss_Attack->Add_Attack_AnimTag("Attack_1");
	pBoss_Attack->Add_Attack_AnimTag("Attack_2");
	pBoss_Attack->Add_Attack_AnimTag("Attack_3");
	pBoss_Attack->Add_Attack_AnimTag("Attack_4");
	pBoss_Attack->Add_Attack_AnimTag("Attack_5");

	pAction_Rest->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsSpawn = { nullptr };
			pBlackBoard->Get_Type(L"isSpawn", pIsSpawn);
			if (nullptr == pIsSpawn)
				return false;

			return !(*pIsSpawn);
		});

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Action_Rest", pAction_Rest)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Action_Hit", pAction_Hit)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Pattern_Attack", pBoss_Attack)))
		return E_FAIL;

	if (FAILED(pAction_Hit->Assemble_Childs()))
		return E_FAIL;
	if (FAILED(pBoss_Attack->Assemble_Childs()))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CHollowLord* CHollowLord::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHollowLord* pInstance = new CHollowLord(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHollowLord");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CHollowLord* CHollowLord::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CHollowLord* pInstance = new CHollowLord(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CHollowLord");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHollowLord::Free()
{
	CMonster::Free();
}
