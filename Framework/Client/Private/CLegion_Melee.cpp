
#include "CLegion_Melee.h"

#include "CBlackBoard.h"
#include "MonoBehavior_Defines.h"
#include "CGameInstance.h"
#include "CPlayer.h"
#include "CWeapon.h"
#include "CSoul.h"

CLegion_Melee::CLegion_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CLegion_Melee::CLegion_Melee(const CLegion_Melee& rhs)
	: CMonster(rhs)
{
}

HRESULT CLegion_Melee::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CMonster::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	m_pAttack->Set_Damage(1);
	m_pDeffence->Set_Deffence(0);
	m_pHealth->Set_Max_Hp(7);
	m_pHealth->Set_HP(7);

	return S_OK;
}

void CLegion_Melee::Tick(const _double& TimeDelta)
{
	CMonster::Tick(TimeDelta);
}

void CLegion_Melee::AfterFrustumTick(const _double& TimeDelta)
{
	CMonster::AfterFrustumTick(TimeDelta);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CLegion_Melee::Late_Tick(const _double& TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);
}

HRESULT CLegion_Melee::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CLegion_Melee::Dead_Motion(const _double& TimeDelta)
{
	CMonster::Dead_Motion(TimeDelta);

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

	if (true == m_pModelCom->isFinishedAnimation())
		m_isRender = false;

	if (0 == m_vecSouls.size())
		m_isRemove = true;
}

void CLegion_Melee::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);

	if (Collision.pMyCollider->Get_Tag() == L"Col_Attack" &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		//cout << "데미지" << endl;
		Collision.pOther->Get_Damaged(m_pAttack);
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isSpawn = true;
	}
}

void CLegion_Melee::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
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

void CLegion_Melee::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
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

HRESULT CLegion_Melee::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_Legion",
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

	AABBDesc.vExtents = _float3(1.f, 0.5f, 0.5f);
	AABBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	AABBDesc.eGroup = CCollider::COL_ENEMY_ATK;
	AABBDesc.vOffset = _float3(0.f, 0.5f, 1.5f);
	AABBDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Col_Attack", &AABBDesc)))
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

	/* Setup Notify */
	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLegion_Melee::Add_Parts()
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

	Desc.wstrModelTag = L"Model_Legion_Weapon";

	if (FAILED(CGameObject::Add_Parts(LEVEL_GAMEPLAY, L"Weapon", L"Weapon_Legion", this, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLegion_Melee::Make_AI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;

	CAction_Hit* pHit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Action_Hit", this));
	if (nullptr == pHit)
		return E_FAIL;
	CPattern_Attack* pPattern_Attack = dynamic_cast<CPattern_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Pattern_Attack", this));
	if (nullptr == pPattern_Attack)
		return E_FAIL;

	pPattern_Attack->Bind_FollowAnimTag("Run");
	pPattern_Attack->Add_Attack_AnimTag("Attack_1");
	pPattern_Attack->Add_Attack_AnimTag("Attack_2");
	pPattern_Attack->Add_Attack_AnimTag("Attack_3");

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Action_Hit", pHit)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Pattern_Attack", pPattern_Attack)))
		return E_FAIL;

	pHit->Assemble_Childs();
	pPattern_Attack->Assemble_Childs();

	Safe_Release(pGameInstance);

	return S_OK;
}

CLegion_Melee* CLegion_Melee::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLegion_Melee* pInstance = new CLegion_Melee(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLegion_Melee");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CLegion_Melee* CLegion_Melee::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CLegion_Melee* pInstance = new CLegion_Melee(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CLegion_Melee");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLegion_Melee::Free()
{
	for (auto& pSoul : m_vecSouls)
		Safe_Release(pSoul);

	CMonster::Free();
}
