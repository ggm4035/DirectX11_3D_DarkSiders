
#include "CLegion_Champion.h"

#include "CBlackBoard.h"
#include "MonoBehavior_Defines.h"
#include "CGameInstance.h"
#include "CPlayer.h"
#include "CWeapon.h"

CLegion_Champion::CLegion_Champion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CLegion_Champion::CLegion_Champion(const CLegion_Champion& rhs)
	: CMonster(rhs)
{
}

HRESULT CLegion_Champion::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CMonster::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	m_pTransformCom->Get_Scaled();

	m_Status.iHP = 10;
	m_Status.iMaxHP = 10;

	return S_OK;
}

void CLegion_Champion::Tick(const _double& TimeDelta)
{
	CMonster::Tick(TimeDelta);
}

void CLegion_Champion::AfterFrustumTick(const _double& TimeDelta)
{
	CMonster::AfterFrustumTick(TimeDelta);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CLegion_Champion::Late_Tick(const _double& TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);
}

HRESULT CLegion_Champion::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CLegion_Champion::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
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

void CLegion_Champion::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
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

void CLegion_Champion::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
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

HRESULT CLegion_Champion::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_Legion_Champion",
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	CBounding_AABB::AABBDESC AABBDesc;
	CBounding_Sphere::SPHEREDESC SphereDesc;

	SphereDesc.fRadius = 1.f;
	SphereDesc.vPosition = _float3(0.f, 1.f, 0.f);
	SphereDesc.eGroup = CCollider::COL_ENEMY;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Body", &SphereDesc)))
		return E_FAIL;

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

	SphereDesc.fRadius = 1.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);
	SphereDesc.eGroup = CCollider::COL_ENEMY_ATK;
	SphereDesc.vOffset = _float3(0.f, 0.5f, 1.5f);
	SphereDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Attack", &SphereDesc)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLegion_Champion::Make_AI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;

	CAction_Hit* pHit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence_Hit", this));
	if (nullptr == pHit)
		return E_FAIL;

	CPattern_Attack* pPattern_Attack = dynamic_cast<CPattern_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Pattern_Attack", this));
	if (nullptr == pPattern_Attack)
		return E_FAIL;

	CWait* pWait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Wait", this));
	if (nullptr == pWait)
		return E_FAIL;

	pWait->Set_Timer(1.f);

	pWait->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CGameObject3D::HITSTATE* pCurState = { nullptr };
			pBlackBoard->Get_Type(L"eCurHitState", pCurState);

			if (CGameObject3D::NONE == *pCurState)
				return true;
			else
				return false;
		});
	pPattern_Attack->Bind_FollowAnimTag("Run");
	pPattern_Attack->Add_Attack_AnimTag("Attack_1");
	pPattern_Attack->Add_Attack_AnimTag("Attack_2");
	pPattern_Attack->Add_Attack_AnimTag("Attack_3");

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Hit", pHit)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Pattern_Attack", pPattern_Attack)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Wait", pWait)))
		return E_FAIL;

	pHit->Assemble_Childs();
	pPattern_Attack->Assemble_Childs();

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLegion_Champion::Add_Parts()
{
	CWeapon::WEAPONDESC Desc;
	const CBone* pBone = m_pModelCom->Get_Bone("Bone_LE_Weapon");
	if (nullptr == pBone)
		return E_FAIL;

	XMStoreFloat4x4(&Desc.OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&Desc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	Desc.pCombinedTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	Desc.pNotMoveCombinedTransformationMatrix = pBone->Get_NotMoveCombinedTransformationMatrixPtr();
	Desc.pWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	Desc.SpeedPerSec = 7.0;
	Desc.RotationPerSec = XMConvertToRadians(90.0);

	Desc.wstrModelTag = L"Model_Legion_Champion_Weapon_A";

	if (FAILED(CGameObject::Add_Parts(LEVEL_GAMEPLAY, L"Weapon", L"Weapon_Legion_Champion_A", this, &Desc)))
		return E_FAIL;

	return S_OK;
}

CLegion_Champion* CLegion_Champion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLegion_Champion* pInstance = new CLegion_Champion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLegion_Champion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CLegion_Champion* CLegion_Champion::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CLegion_Champion* pInstance = new CLegion_Champion(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CLegion_Champion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLegion_Champion::Free()
{
	CMonster::Free();
}
