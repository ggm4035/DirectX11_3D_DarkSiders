#include "CSteamRoller.h"

#include "MonoBehavior_Defines.h"
#include "CBlackBoard.h"
#include "CGameInstance.h"
#include "CPlayer.h"
#include "CWeapon.h"

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

	XMStoreFloat4(&m_vResponPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_Status.iHP = 30;
	m_Status.iMaxHP = 30;

	return S_OK;
}

void CSteamRoller::Tick(const _double& TimeDelta)
{
	CGameObject3D::Tick(TimeDelta);

	m_pTransformCom->Animation_Movement(m_pModelCom, TimeDelta);

	m_fTimeAcc += TimeDelta;

	m_pRoot->Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta, m_pNavigationCom);

	Tick_Colliders(m_pTransformCom->Get_WorldMatrix());
}

void CSteamRoller::AfterFrustumTick(const _double& TimeDelta)
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

#ifdef _DEBUG
		if (true == m_isRender && FAILED(Add_Colliders_Debug_Render_Group(m_pRendererCom)))
			return;
#endif
	}

	Safe_Release(pGameInstance);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CSteamRoller::Late_Tick(const _double& TimeDelta)
{
	On_Colisions(TimeDelta);
}

HRESULT CSteamRoller::Render()
{
	if (FAILED(CMonster::Render(0)))
		return E_FAIL;

	return S_OK;
}

void CSteamRoller::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);

	if (Collision.pMyCollider->Get_Tag() == L"Col_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isSpawn = true;
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

	/* Col_Body */
	AABBDesc.vExtents = _float3(2.f, 2.f, 2.f);
	AABBDesc.eGroup = CCollider::COL_ENEMY;
	AABBDesc.vOffset = _float3(0.f, 2.f, 0.f);
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Col_Body", &AABBDesc)))
		return E_FAIL;

	/* Col_Range */
	SphereDesc.fRadius = 20.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);
	SphereDesc.eGroup = CCollider::COL_ENEMY_RANGE;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Range", &SphereDesc)))
		return E_FAIL;

	/* Col_Attack_Range */
	SphereDesc.fRadius = 5.f;
	SphereDesc.eGroup = CCollider::COL_ENEMY_MELEE_RANGE;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Melee_Range", &SphereDesc)))
		return E_FAIL;

	/* Add_Col_Attack */
	AABBDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	AABBDesc.eGroup = CCollider::COL_ENEMY_ATK;
	AABBDesc.vOffset = _float3(0.f, 0.5f, 1.f);
	AABBDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Col_Attack", &AABBDesc)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	return S_OK;
}

/* 보스임 */
HRESULT CSteamRoller::Make_AI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Root */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Root", L"Com_Root",
		(CComponent**)&m_pRoot, this)))
		return E_FAIL;

	/* BlackBoard */
	m_pRoot->Add_Type(L"vDirection", _float3());

	m_pRoot->Add_Type(L"eCurHitState", &m_eCurHitState);

	m_pRoot->Add_Type(L"isDead", &m_isDead);
	m_pRoot->Add_Type(L"isSpawn", &m_isSpawn);
	m_pRoot->Add_Type(L"isRemove", &m_isRemove);
	m_pRoot->Add_Type(L"isSpawnEnd", &m_isSpawnEnd);
	m_pRoot->Add_Type(L"isAbleAttack", &m_isAbleAttack);
	m_pRoot->Add_Type(L"isRangeInPlayer", &m_isRangeInPlayer);

	m_pRoot->Add_Type(L"fCoolTime", &m_fCoolTime);
	m_pRoot->Add_Type(L"fTimeAcc", &m_fTimeAcc);

	m_pRoot->Add_Type(L"pTarget", pGameInstance->Get_Player());

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;

	CDetect* pDetect = dynamic_cast<CDetect*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Detect", this));
	if (nullptr == pDetect)
		return E_FAIL;
	CAction_Hit* pSequence_Hit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence_Hit", this));
	if (nullptr == pSequence_Hit)
		return E_FAIL;
	CPattern* pPattern = dynamic_cast<CPattern*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Pattern", this));
	if (nullptr == pPattern)
		return E_FAIL;
	CFollow* pFollow = dynamic_cast<CFollow*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Follow", this));
	if (nullptr == pFollow)
		return E_FAIL;
	CAction_Attack* pSelector_Attack = dynamic_cast<CAction_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector_Attack", this));
	if (nullptr == pSelector_Attack)
		return E_FAIL;
	CWait* pWait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Wait", this));
	if (nullptr == pWait)
		return E_FAIL;

	pWait->Set_LimitTime(1.f);

	pWait->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CGameObject3D::HITSTATE* pCurState = { nullptr };
			pBlackBoard->Get_Type(L"eCurHitState", pCurState);

			if (CGameObject3D::NONE == *pCurState)
				return true;
			else
				return false;
		});

	pPattern->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_float* pTimeAcc = { nullptr };
			_float* pCoolTime = { nullptr };

			pBlackBoard->Get_Type(L"fTimeAcc", pTimeAcc);
			pBlackBoard->Get_Type(L"fCoolTime", pCoolTime);

			if (*pTimeAcc < *pCoolTime)
				return false;
			else
				return true;
		});

	pDetect->Bind_AnimTag("Enrage");

	pFollow->Bind_AnimationTag("Rolling");
	pFollow->Bind_Move_Speed(2.f);
	pFollow->Bind_Turn_Speed(0.5f);
	pFollow->Set_Timer(12.f);
	pPattern->Bind_AnimationTag("Roll_Start", "Rolling", "Roll_Stop");
	
	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Detect", pDetect)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Test", pPattern)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Hit", pSequence_Hit)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Selector_Attack", pSelector_Attack)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Wait", pWait)))
		return E_FAIL;

	if (FAILED(pPattern->Assemble_Behavior(L"Tsk_Pattern_Roll", pFollow)))
		return E_FAIL;
	
	if (FAILED(pSequence_Hit->Assemble_Childs()))
		return E_FAIL;
	if (FAILED(pSelector_Attack->Assemble_Childs()))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CSteamRoller* CSteamRoller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSteamRoller* pInstance = new CSteamRoller(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTusker");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CSteamRoller* CSteamRoller::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CSteamRoller* pInstance = new CSteamRoller(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CTusker");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSteamRoller::Free()
{
	CMonster::Free();
}
