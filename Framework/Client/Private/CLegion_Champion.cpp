
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

	m_Status.iHP = 10;
	m_Status.iMaxHP = 10;

	return S_OK;
}

void CLegion_Champion::Tick(const _double& TimeDelta)
{
	CGameObject3D::Tick(TimeDelta);

	m_pTransformCom->Animation_Movement(m_pModelCom, TimeDelta);

	m_pRoot->Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta, m_pNavigationCom);

	Tick_Colliders(m_pTransformCom->Get_WorldMatrix());
}

void CLegion_Champion::AfterFrustumTick(const _double& TimeDelta)
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

		for (auto Pair : m_Parts)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, static_cast<CGameObject*>(Pair.second));

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
void CLegion_Champion::Late_Tick(const _double& TimeDelta)
{
	On_Colisions(TimeDelta);
}

HRESULT CLegion_Champion::Render()
{
	if (FAILED(CMonster::Render(0)))
		return E_FAIL;

	return S_OK;
}

void CLegion_Champion::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);

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

	AABBDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	AABBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	AABBDesc.eGroup = CCollider::COL_ENEMY_ATK;
	AABBDesc.vOffset = _float3(0.f, 0.5f, 1.5f);
	AABBDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Col_Attack", &AABBDesc)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLegion_Champion::Make_AI()
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

	m_pRoot->Add_Type(L"pTarget", pGameInstance->Get_Player());

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;

	//CSpawn* pSpawn = dynamic_cast<CSpawn*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Spawn", this));
	//if (nullptr == pSpawn)
	//	return E_FAIL;

	CAction_Hit* pHit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence_Hit", this));
	if (nullptr == pHit)
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

	//if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Spawn", pSpawn)))
	//	return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Hit", pHit)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Selector_Attack", pAttack)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Wait", pWait)))
		return E_FAIL;

	pHit->Assemble_Childs();
	pAttack->Assemble_Childs();

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
