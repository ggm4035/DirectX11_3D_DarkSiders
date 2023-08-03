#include "CFallenDog.h"

#include "MonoBehavior_Defines.h"
#include "CBlackBoard.h"
#include "CGameInstance.h"
#include "CPlayer.h"

CFallenDog::CFallenDog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CFallenDog::CFallenDog(const CFallenDog& rhs)
	: CMonster(rhs)
{
}

HRESULT CFallenDog::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CMonster::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4(&m_vResponPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_Status.iHP = 25;
	m_Status.iMaxHP = 25;

	return S_OK;
}

void CFallenDog::Tick(const _double& TimeDelta)
{
	CMonster::Tick(TimeDelta);
}

void CFallenDog::AfterFrustumTick(const _double& TimeDelta)
{
	CMonster::AfterFrustumTick(TimeDelta);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CFallenDog::Late_Tick(const _double& TimeDelta)
{
	On_Colisions(TimeDelta);
}

HRESULT CFallenDog::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CFallenDog::Dead_Motion(const _double& TimeDelta)
{
	CMonster::Dead_Motion(TimeDelta);

	if (true == m_pModelCom->isFinishedAnimation())
		m_isRemove = true;
}

void CFallenDog::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);

	if (Collision.pMyCollider->Get_Tag() == L"Col_Attack" &&
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

	if (Collision.pMyCollider->Get_Tag() == L"Col_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isSpawn = true;
	}
}

void CFallenDog::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
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

void CFallenDog::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
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

HRESULT CFallenDog::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_FallenDog",
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	CBounding_Sphere::SPHEREDESC SphereDesc;
	CBounding_AABB::AABBDESC AABBDesc;

	/* Col_Body */
	AABBDesc.vExtents = _float3(1.5f, 1.5f, 1.5f);
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
	SphereDesc.fRadius = 8.f;
	SphereDesc.eGroup = CCollider::COL_ENEMY_MELEE_RANGE;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Melee_Range", &SphereDesc)))
		return E_FAIL;

	/* Add_Col_Attack */
	AABBDesc.vExtents = _float3(2.f, 0.5f, 2.f);
	AABBDesc.eGroup = CCollider::COL_ENEMY_ATK;
	AABBDesc.vOffset = _float3(0.f, 0.5f, 2.f);
	AABBDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Col_Attack", &AABBDesc)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	return S_OK;
}

/* 준보스임 */
HRESULT CFallenDog::Make_AI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;
	CSequence* pSequence_Spawn = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence", this));
	if (nullptr == pSequence_Spawn)
		return E_FAIL;
	CSequence* pSequence_Hit = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence", this));
	if (nullptr == pSequence_Hit)
		return E_FAIL;

	CAction* pAction_Spawn = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Spawn)
		return E_FAIL;
	CAction* pAction_Breath = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Breath)
		return E_FAIL;
	CHit* pHit = dynamic_cast<CHit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Hit", this));
	if (nullptr == pHit)
		return E_FAIL;
	CAction* pAction_Dead = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Dead)
		return E_FAIL;
	CPattern_Attack* pPattern_Attack = dynamic_cast<CPattern_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Pattern_Attack", this));
	if (nullptr == pPattern_Attack)
		return E_FAIL;

	CDead* pDead = dynamic_cast<CDead*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Dead", this));
	if (nullptr == pDead)
		return E_FAIL;

	pAction_Spawn->Bind_AnimationTag("Spawn");
	pAction_Spawn->Just_One_Time_Action();
	pAction_Breath->Bind_AnimationTag("Breath");
	pHit->Not_Impact();
	pAction_Dead->Bind_AnimationTag("Finish");

	pPattern_Attack->Bind_FollowAnimTag("Walk_F");
	pPattern_Attack->Add_Attack_AnimTag("Attack_1");
	pPattern_Attack->Add_Attack_AnimTag("Attack_2");
	pPattern_Attack->Add_Attack_AnimTag("Attack_Head");
	pPattern_Attack->Add_Attack_AnimTag("Attack_Slam");
	pPattern_Attack->Add_Attack_AnimTag("3Hit");

	pSequence_Spawn->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsSpawn = { nullptr };
			pBlackBoard->Get_Type(L"isSpawn", pIsSpawn);
			if (nullptr == pIsSpawn)
				return false;

			return *pIsSpawn;
		});

	pAction_Dead->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsDead = { nullptr };
			pBlackBoard->Get_Type(L"isDead", pIsDead);
			if (nullptr == pIsDead)
				return false;

			return *pIsDead;
		});

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Spawn", pSequence_Spawn)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Action_Hit", pSequence_Hit)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Pattern_Attack", pPattern_Attack)))
		return E_FAIL;

	if (FAILED(pSequence_Spawn->Assemble_Behavior(L"Action_Spawn", pAction_Spawn)))
		return E_FAIL;
	if (FAILED(pSequence_Spawn->Assemble_Behavior(L"Action_Breath", pAction_Breath)))
		return E_FAIL;
	if (FAILED(pSequence_Hit->Assemble_Behavior(L"Tsk_Hit", pHit)))
		return E_FAIL;
	if (FAILED(pSequence_Hit->Assemble_Behavior(L"Action_Dead", pAction_Dead)))
		return E_FAIL;

	if (FAILED(pAction_Dead->Assemble_Behavior(L"Tsk_Dead", pDead)))
		return E_FAIL;

	if (FAILED(pPattern_Attack->Assemble_Childs()))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CFallenDog* CFallenDog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFallenDog* pInstance = new CFallenDog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CFallenDog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CFallenDog* CFallenDog::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CFallenDog* pInstance = new CFallenDog(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CFallenDog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFallenDog::Free()
{
	CMonster::Free();
}
