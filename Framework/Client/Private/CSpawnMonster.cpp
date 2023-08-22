#include "CSpawnMonster.h"

#include "MonoBehavior_Defines.h"
#include "CBlackBoard.h"
#include "CGameInstance.h"
#include "CPlayer.h"

#include "CAoE.h"
#include "CUI_Sprite.h"
#include "CSoul.h"

CSpawnMonster::CSpawnMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CSpawnMonster::CSpawnMonster(const CSpawnMonster& rhs)
	: CMonster(rhs)
{
}

HRESULT CSpawnMonster::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CMonster::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, static_cast<SPAWNDESC*>(pArg)->wstrModelTag, 
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4(&m_vResponPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pAttack->Set_Damage(30);
	m_pDeffence->Set_Deffence(10);
	m_pHealth->Set_Max_Hp(100);
	m_pHealth->Set_HP(100);

	return S_OK;
}

void CSpawnMonster::Tick(const _double& TimeDelta)
{
	if (false == m_isSpawn)
	{
		_vector vPosition = XMLoadFloat4(&m_vResponPosition);

		vPosition.m128_f32[1] = vPosition.m128_f32[1] - 2.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

		m_isSpawn = true;
	}

	CMonster::Tick(TimeDelta);
}

void CSpawnMonster::AfterFrustumTick(const _double& TimeDelta)
{
	if (false == m_isRender)
		return;

	CMonster::AfterFrustumTick(TimeDelta);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CSpawnMonster::Late_Tick(const _double& TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);
}

HRESULT CSpawnMonster::Render()
{
	m_iPassNum = 0;

	if (CHealth::HIT_NONE != m_pHealth->Get_Current_HitState())
		m_iPassNum = 1;
	if (true == m_pHealth->isDead())
		m_iPassNum = 3;

	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CSpawnMonster::Dead_Motion(const _double& TimeDelta)
{
	CMonster::Dead_Motion(TimeDelta);

	if (false == m_isRender)
	{
		for (auto iter = m_vecSouls.begin(); iter != m_vecSouls.end();)
		{
			(*iter)->Tick(TimeDelta);
			if (true == (*iter)->is_Remove())
			{
				Safe_Release(*iter);
				iter = m_vecSouls.erase(iter);
			}

			else
				++iter;
		}

		for (auto& pSoul : m_vecSouls)
			pSoul->AfterFrustumTick(TimeDelta);

		if (0 == m_vecSouls.size())
			m_isRemove = true;
	}
}

void CSpawnMonster::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (Collision.pOtherCollider->Get_Tag() == L"Col_Body" &&
		Collision.pMyCollider->Get_Tag() == L"Col_Body")
	{
		_vector vOtherPosition = Collision.pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vOtherPosition);
		m_pTransformCom->Repersive(vDir, TimeDelta);
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Attack" &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		Collision.pOther->Get_Damaged(m_pAttack);
	}
}

void CSpawnMonster::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if ((Collision.pOtherCollider->Get_Collider_Group() == CCollider::COL_STATIC ||
		Collision.pOtherCollider->Get_Tag() == L"Col_Body") &&
		Collision.pMyCollider->Get_Tag() == L"Col_Body")
	{
		_vector vOtherPosition = Collision.pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vOtherPosition);
		m_pTransformCom->Repersive(vDir, TimeDelta);
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Melee_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isAbleAttack = true;
	}
}

void CSpawnMonster::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (Collision.pMyCollider->Get_Tag() == L"Col_Melee_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isAbleAttack = false;
	}
}

HRESULT CSpawnMonster::Add_Components()
{
	CBounding_Sphere::SPHEREDESC SphereDesc;
	/* Col_Body */
	SphereDesc.fRadius = 1.f;
	SphereDesc.vPosition = _float3(0.f, 1.f, 0.f);
	SphereDesc.eGroup = CCollider::COL_ENEMY;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Body", &SphereDesc)))
		return E_FAIL;

	/* Col_Attack_Range */
	SphereDesc.fRadius = 5.f;
	SphereDesc.eGroup = CCollider::COL_ENEMY_MELEE_RANGE;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Melee_Range", &SphereDesc)))
		return E_FAIL;

	/* Col_Attack */
	SphereDesc.fRadius = 1.f;
	SphereDesc.eGroup = CCollider::COL_ENEMY_ATK;
	SphereDesc.vOffset = _float3(0.f, 1.f, 1.f);
	SphereDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Attack", &SphereDesc)))
		return E_FAIL;

	CSoul::SOULDESC tSoulDesc;
	tSoulDesc.RotationPerSec = XMConvertToRadians(90.f);
	tSoulDesc.SpeedPerSec = 2.f;

	for (_uint i = 0; i < 4; ++i)
	{
		CSoul* pSoul = { nullptr };
		wstring wstrTag = L"Soul" + to_wstring(i);
		if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Soul", wstrTag,
			(CComponent**)&pSoul, this, &tSoulDesc)))
			return E_FAIL;

		m_vecSouls.push_back(pSoul);
	}

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	return S_OK;
}

/* 평소에는 근접공격, 일정 체력 이하로 떨어지면 자폭돌진 */
HRESULT CSpawnMonster::Make_AI()
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

	CRandomLook* pRandomLook = dynamic_cast<CRandomLook*>(pGameInstance->
		Clone_Component(LEVEL_STATIC, L"Tsk_RandomLook", this));
	if (nullptr == pRandomLook)
		return E_FAIL;
	CAction* pAction_Spawn = dynamic_cast<CAction*>(pGameInstance->
		Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Spawn)
		return E_FAIL;
	CHit* pHit = dynamic_cast<CHit*>(pGameInstance->
		Clone_Component(LEVEL_STATIC, L"Tsk_Hit", this));
	if (nullptr == pHit)
		return E_FAIL;
	CPattern_Attack* pPattern_Attack = dynamic_cast<CPattern_Attack*>(pGameInstance->
		Clone_Component(LEVEL_STATIC, L"Pattern_Attack", this));
	if (nullptr == pPattern_Attack)
		return E_FAIL;

	CJump* pJump = dynamic_cast<CJump*>(pGameInstance->
		Clone_Component(LEVEL_STATIC, L"Tsk_Jump", this));
	if (nullptr == pJump)
		return E_FAIL;
	CMove* pMove = dynamic_cast<CMove*>(pGameInstance->
		Clone_Component(LEVEL_STATIC, L"Tsk_Move", this));
	if (nullptr == pMove)
		return E_FAIL;
	CFollow* pFollow = dynamic_cast<CFollow*>(pGameInstance->
		Clone_Component(LEVEL_STATIC, L"Tsk_Follow", this));
	if (nullptr == pFollow)
		return E_FAIL;
	CDead* pDead = dynamic_cast<CDead*>(pGameInstance->
		Clone_Component(LEVEL_STATIC, L"Tsk_Dead", this));
	if (nullptr == pDead)
		return E_FAIL;

	pHit->Bind_SoundTag(L"en_fleamag_impact_01.ogg");
	pAction_Spawn->Bind_AnimationTag("Spawn");
	pAction_Spawn->Just_One_Time_Action();
	pPattern_Attack->Bind_FollowAnimTag("Run");
	pPattern_Attack->Add_Attack_AnimTag("Attack_1");
	pPattern_Attack->Add_Attack_AnimTag("Attack_2");
	pJump->Bind_Jump_Force(0.7f);
	pMove->Bind_Move_Force(0.5f);
	pFollow->Set_Timer(5.f);
	pFollow->Bind_Move_Speed(1.5f);

	pAction_Spawn->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsSpawn = { nullptr };
			pBlackBoard->Get_Type(L"isSpawn", pIsSpawn);
			if (nullptr == pIsSpawn)
				return false;

			return *pIsSpawn;
		});

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Spawn", pSequence_Spawn)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Hit", pHit)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Pattern_Attack", pPattern_Attack)))
		return E_FAIL;

	if (FAILED(pSequence_Spawn->Assemble_Behavior(L"Tsk_RandomLook", pRandomLook)))
		return E_FAIL;
	if (FAILED(pSequence_Spawn->Assemble_Behavior(L"Action_Spawn", pAction_Spawn)))
		return E_FAIL;

	if (FAILED(pAction_Spawn->Assemble_Behavior(L"Tsk_Jump", pJump)))
		return E_FAIL;
	if (FAILED(pAction_Spawn->Assemble_Behavior(L"Tsk_Move", pMove)))
		return E_FAIL;

	if (FAILED(pPattern_Attack->Assemble_Childs()))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CSelector* CSpawnMonster::Make_Attack()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return nullptr;

	CSequence* pSequence_Attack = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence", this));
	if (nullptr == pSequence_Attack)
		return nullptr;
	CRandomSelector* pRandom_Attack = dynamic_cast<CRandomSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"RandomSelector", this));
	if (nullptr == pRandom_Attack)
		return nullptr;

	/* Action */
	CAction* pAction_Follow = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Follow)
		return nullptr;
	CAction* pAction_LookAt = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_LookAt)
		return nullptr;

	CAction* pAction_Attack1 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Attack1)
		return nullptr;
	CAction* pAction_Attack2 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Attack2)
		return nullptr;
	CAction* pAction_Attack3 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Attack3)
		return nullptr;

	/* Movement */
	CFollow* pFollow = dynamic_cast<CFollow*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Follow", this));
	if (nullptr == pFollow)
		return nullptr;
	CLookAtTarget* pLookAtTarget = dynamic_cast<CLookAtTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_LookAtTarget", this));
	if (nullptr == pLookAtTarget)
		return nullptr;

	pSelector->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CHealth* pHealth = { nullptr };
			pBlackBoard->Get_Type(L"pHealth", pHealth);
			if (nullptr == pHealth)
				return false;

			if (CHealth::HIT_NONE != pHealth->Get_Current_HitState())
				return false;

			return true;
		});

	pAction_Follow->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsAbleAttack = { nullptr };

			pBlackBoard->Get_Type(L"isAbleAttack", pIsAbleAttack);
			if (nullptr == pIsAbleAttack)
				return false;

			return !(*pIsAbleAttack);
		});

	pAction_Follow->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			if (true == m_pModelCom->isLoopAnimation() ||
				true == m_pModelCom->isFinishedAnimation())
				return true;

			return false;
		});

	pAction_Follow->Bind_AnimationTag("Run");
	pAction_LookAt->Bind_AnimationTag("Run");

	pFollow->Bind_Move_Speed(0.9f);
	pFollow->Bind_Turn_Speed(5.f);
	pLookAtTarget->Set_Timer(1.5f);

	pAction_Attack1->Bind_AnimationTag("Attack_1");
	pAction_Attack2->Bind_AnimationTag("Attack_2");
	pAction_Attack3->Bind_AnimationTag("Attack_3");

	if (FAILED(pSelector->Assemble_Behavior(L"Action_Follow", pAction_Follow)))
		return nullptr;
	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Attack", pSequence_Attack)))
		return nullptr;

	if (FAILED(pAction_Follow->Assemble_Behavior(L"Tsk_Follow", pFollow)))
		return nullptr;
	if (FAILED(pSequence_Attack->Assemble_Behavior(L"Action_LookAt", pAction_LookAt)))
		return nullptr;
	if (FAILED(pSequence_Attack->Assemble_Behavior(L"Random_Attack", pRandom_Attack)))
		return nullptr;

	if (FAILED(pAction_LookAt->Assemble_Behavior(L"Tsk_LookAtTarget", pLookAtTarget)))
		return nullptr;
	if (FAILED(pRandom_Attack->Assemble_Behavior(pGameInstance->strToWStr("Attack_1"), pAction_Attack1)))
		return nullptr;
	if (FAILED(pRandom_Attack->Assemble_Behavior(pGameInstance->strToWStr("Attack_2"), pAction_Attack2)))
		return nullptr;
	if (FAILED(pRandom_Attack->Assemble_Behavior(pGameInstance->strToWStr("Attack_3"), pAction_Attack3)))
		return nullptr;

	Safe_Release(pGameInstance);

	return pSelector;
}

CSpawnMonster* CSpawnMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpawnMonster* pInstance = new CSpawnMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSpawnMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CSpawnMonster* CSpawnMonster::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CSpawnMonster* pInstance = new CSpawnMonster(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CSpawnMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSpawnMonster::Free()
{
	for (auto& pSoul : m_vecSouls)
		Safe_Release(pSoul);

	CMonster::Free();
}
