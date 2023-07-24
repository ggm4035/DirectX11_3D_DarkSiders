
#include "CGoblin_Armor.h"

#include "MonoBehavior_Defines.h"
#include "CBlackBoard.h"
#include "CGameInstance.h"
#include "CPlayer.h"
#include "CWeapon.h"

CGoblin_Armor::CGoblin_Armor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CGoblin_Armor::CGoblin_Armor(const CGoblin_Armor& rhs)
	: CMonster(rhs)
{
}

HRESULT CGoblin_Armor::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CMonster::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	XMStoreFloat4(&m_vResponPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_Status.iHP = 4;
	m_Status.iMaxHP = 4;

	return S_OK;
}

void CGoblin_Armor::Tick(const _double& TimeDelta)
{
	CMonster::Tick(TimeDelta);
}

void CGoblin_Armor::AfterFrustumTick(const _double& TimeDelta)
{
	CMonster::AfterFrustumTick(TimeDelta);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CGoblin_Armor::Late_Tick(const _double& TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);
}

HRESULT CGoblin_Armor::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CGoblin_Armor::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
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

void CGoblin_Armor::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
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

void CGoblin_Armor::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
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

HRESULT CGoblin_Armor::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_Goblin_Armor",
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	CBounding_Sphere::SPHEREDESC SphereDesc;
	CBounding_AABB::AABBDESC AABBDesc;
	/* Col_Body */
	SphereDesc.fRadius = 0.5f;
	SphereDesc.vPosition = _float3(0.f, 0.5f, 0.f);
	SphereDesc.eGroup = CCollider::COL_ENEMY;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Body", &SphereDesc)))
		return E_FAIL;

	/* Col_Range */
	SphereDesc.fRadius = 10.f;
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

/* 발견전에는 앉아있다가 플레이어 발견하면 이후부터 경계모드 */
HRESULT CGoblin_Armor::Make_AI()
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

	CRest* pRest = dynamic_cast<CRest*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Rest", this));
	if (nullptr == pRest)
		return E_FAIL;
	CSelector* pSelector_Alert = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector_Alert)
		return E_FAIL;

	CDetect* pDetect = dynamic_cast<CDetect*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Detect", this));
	if (nullptr == pDetect)
		return E_FAIL;
	CAction_Hit* pSequence_Hit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence_Hit", this));
	if (nullptr == pSequence_Hit)
		return E_FAIL;
	CAction_Patrol* pSequence_Patrol = dynamic_cast<CAction_Patrol*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence_Patrol", this));
	if (nullptr == pSequence_Patrol)
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

	pRest->Add_AnimTag("Sit");
	pDetect->Bind_AnimTag("Sit_End");

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Rest", pRest)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Selector_Alert", pSelector_Alert)))
		return E_FAIL;

	if (FAILED(pSelector_Alert->Assemble_Behavior(L"Tsk_Detect", pDetect)))
		return E_FAIL;
	if (FAILED(pSelector_Alert->Assemble_Behavior(L"Sequence_Hit", pSequence_Hit)))
		return E_FAIL;
	if (FAILED(pSelector_Alert->Assemble_Behavior(L"Sequence_Patrol", pSequence_Patrol)))
		return E_FAIL;
	if (FAILED(pSelector_Alert->Assemble_Behavior(L"Selector_Attack", pSelector_Attack)))
		return E_FAIL;
	if (FAILED(pSelector_Alert->Assemble_Behavior(L"Tsk_Wait", pWait)))
		return E_FAIL;

	pSequence_Hit->Assemble_Childs();
	pSequence_Patrol->Assemble_Childs();
	pSelector_Attack->Assemble_Childs("Run");

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CGoblin_Armor::Add_Parts()
{
	CWeapon::WEAPONDESC Desc;
	const CBone* pBone = m_pModelCom->Get_Bone("Bone_Goblin_Weapon_Fleamag_Sword");
	if (nullptr == pBone)
		return E_FAIL;

	XMStoreFloat4x4(&Desc.OffsetMatrix, pBone->Get_OffsetMatrix());
	XMStoreFloat4x4(&Desc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	Desc.pCombinedTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	Desc.pNotMoveCombinedTransformationMatrix = pBone->Get_NotMoveCombinedTransformationMatrixPtr();
	Desc.pWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	Desc.SpeedPerSec = 7.0;
	Desc.RotationPerSec = XMConvertToRadians(90.0);

	Desc.wstrModelTag = L"Model_Goblin_Sword";

	if (FAILED(CGameObject::Add_Parts(LEVEL_GAMEPLAY, L"Weapon", L"Weapon_Goblin", this, &Desc)))
		return E_FAIL;

	pBone = m_pModelCom->Get_Bone("Bone_Goblin_Quiver");
	if (nullptr == pBone)
		return E_FAIL;

	XMStoreFloat4x4(&Desc.OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&Desc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	Desc.pCombinedTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	Desc.pNotMoveCombinedTransformationMatrix = pBone->Get_NotMoveCombinedTransformationMatrixPtr();
	Desc.pWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	Desc.wstrModelTag = L"Model_Goblin_Quiver";

	if (FAILED(CGameObject::Add_Parts(LEVEL_GAMEPLAY, L"Weapon", L"Weapon_Goblin_Quiver", this, &Desc)))
		return E_FAIL;

	return S_OK;
}

CGoblin_Armor* CGoblin_Armor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_Armor* pInstance = new CGoblin_Armor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CGoblin_Armor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGoblin_Armor* CGoblin_Armor::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CGoblin_Armor* pInstance = new CGoblin_Armor(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CGoblin_Armor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGoblin_Armor::Free()
{
	CMonster::Free();
}
