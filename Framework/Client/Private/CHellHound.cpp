
#include "CHellHound.h"

#include "MonoBehavior_Defines.h"
#include "CGameInstance.h"
#include "CBlackBoard.h"
#include "CPlayer.h"

CHellHound::CHellHound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CHellHound::CHellHound(const CHellHound& rhs)
	: CMonster(rhs)
{
}

HRESULT CHellHound::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CMonster::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_Status.iHP = 3;
	m_Status.iMaxHP = 3;

	return S_OK;
}

void CHellHound::Tick(const _double& TimeDelta)
{
	CMonster::Tick(TimeDelta);
}

void CHellHound::AfterFrustumTick(const _double& TimeDelta)
{
	CMonster::AfterFrustumTick(TimeDelta);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CHellHound::Late_Tick(const _double& TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);
}

HRESULT CHellHound::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CHellHound::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);

	if (Collision.pMyCollider->Get_Tag() == L"Col_Attack" &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		Collision.pOther->Get_Damaged();
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isSpawn = true;
	}
}

void CHellHound::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
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

void CHellHound::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
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

HRESULT CHellHound::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_HellHound",
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	CBounding_Sphere::SPHEREDESC SphereDesc;

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

	SphereDesc.fRadius = 10.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);
	SphereDesc.eGroup = CCollider::COL_ENEMY_RANGE;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Range", &SphereDesc)))
		return E_FAIL;

	SphereDesc.fRadius = 1.f;
	SphereDesc.vPosition = _float3(0.f, 1.f, 0.f);
	SphereDesc.vOffset = _float3(0.f, 0.f, 0.5f);
	SphereDesc.eGroup = CCollider::COL_ENEMY_ATK;
	SphereDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Attack", &SphereDesc)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHellHound::Make_AI()
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Root */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Root", L"Com_Root",
		(CComponent**)&m_pRoot, this)))
		return E_FAIL;

	/* BlackBoard */
	m_pRoot->Add_Type(L"vDirection", _float3());

	m_pRoot->Add_Type(L"fHitTimeAcc", &m_fHitTimeAcc);
	m_pRoot->Add_Type(L"eCurHitState", &m_eCurHitState);

	m_pRoot->Add_Type(L"isDead", &m_isDead);
	m_pRoot->Add_Type(L"isSpawn", &m_isSpawn);
	m_pRoot->Add_Type(L"isRemove", &m_isRemove);
	m_pRoot->Add_Type(L"isSpawnEnd", &m_isSpawnEnd);
	m_pRoot->Add_Type(L"isAbleAttack", &m_isAbleAttack);
	m_pRoot->Add_Type(L"isRangeInPlayer", &m_isRangeInPlayer);

	m_pRoot->Add_Type(L"pTarget", pGameInstance->Get_Player());

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;

	CAction_Hit* pHit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence_Hit", this));
	if (nullptr == pHit)
		return E_FAIL;

	CAction_Patrol* pPatrol = dynamic_cast<CAction_Patrol*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence_Patrol", this));
	if (nullptr == pPatrol)
		return E_FAIL;

	CAction_Attack* pAttack = dynamic_cast<CAction_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector_Attack", this));
	if (nullptr == pAttack)
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

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Hit", pHit)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Patrol", pPatrol)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Selector_Attack", pAttack)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Wait", pWait)))
		return E_FAIL;

	pHit->Assemble_Childs();
	pAttack->Assemble_Childs("Run");
	pPatrol->Assemble_Childs();

	Safe_Release(pGameInstance);

	return S_OK;
}

CHellHound* CHellHound::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHellHound* pInstance = new CHellHound(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHellHound");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CHellHound* CHellHound::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CHellHound* pInstance = new CHellHound(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CHellHound");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHellHound::Free()
{
	CMonster::Free();
}
