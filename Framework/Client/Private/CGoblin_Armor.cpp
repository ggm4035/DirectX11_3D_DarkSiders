
#include "CGoblin_Armor.h"

#include "MonoBehavior_Defines.h"
#include "CBlackBoard.h"
#include "CGameInstance.h"
#include "CPlayer.h"
#include "CWeapon.h"

#include "CSoul.h"

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

	m_pAttack->Set_Damage(20);
	m_pDeffence->Set_Deffence(5);
	m_pHealth->Set_Max_Hp(100);
	m_pHealth->Set_HP(100);

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
	m_iPassNum = 0;

	if (CHealth::HIT_NONE != m_pHealth->Get_Current_HitState())
		m_iPassNum = 1;
	if (true == m_pHealth->isDead())
		m_iPassNum = 3;

	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CGoblin_Armor::Dead_Motion(const _double& TimeDelta)
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

void CGoblin_Armor::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);
}

void CGoblin_Armor::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionStay(Collision, TimeDelta);
}

void CGoblin_Armor::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionExit(Collision, TimeDelta);
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
	SphereDesc.fRadius = 2.f;
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

/* 발견전에는 앉아있다가 플레이어 발견하면 이후부터 경계모드 */
HRESULT CGoblin_Armor::Make_AI()
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
	CSelector* pSelector_Alert = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector_Alert)
		return E_FAIL;

	CAction* pAction_Detect = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Detect)
		return E_FAIL;
	CAction_Hit* pSequence_Hit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Action_Hit", this));
	if (nullptr == pSequence_Hit)
		return E_FAIL;
	CPattern_Patrol* pSequence_Patrol = dynamic_cast<CPattern_Patrol*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence_Patrol", this));
	if (nullptr == pSequence_Patrol)
		return E_FAIL;
	CPattern_BackDash* pPattern_BackDash = dynamic_cast<CPattern_BackDash*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Pattern_BackDash", this));
	if (nullptr == pPattern_BackDash)
		return E_FAIL;
	CPattern_Attack* pPattern_Attack = dynamic_cast<CPattern_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Pattern_Attack", this));
	if (nullptr == pPattern_Attack)
		return E_FAIL;

	pAction_Rest->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsSpawn = { nullptr };
			pBlackBoard->Get_Type(L"isSpawn", pIsSpawn);
			if (nullptr == pIsSpawn)
				return false;

			return !(*pIsSpawn);
		});

	pSequence_Hit->Bind_SoundTag(L"en_fleamag_armored_block_01.ogg");
	pAction_Rest->Bind_AnimationTag("Sit");
	pAction_Detect->Bind_AnimationTag("Sit_End");
	pAction_Detect->Just_One_Time_Action();
	pPattern_Attack->Bind_FollowAnimTag("Run");
	CPattern_Attack::ATTACKDESC Desc;
	Desc.strAttackAnimTag = "Attack_1";
	pPattern_Attack->Add_Attack(Desc);
	Desc.strAttackAnimTag = "Attack_2"; /* Sound 추가 */
	pPattern_Attack->Add_Attack(Desc);

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Action_Rest", pAction_Rest)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Selector_Alert", pSelector_Alert)))
		return E_FAIL;

	if (FAILED(pSelector_Alert->Assemble_Behavior(L"Action_Detect", pAction_Detect)))
		return E_FAIL;
	if (FAILED(pSelector_Alert->Assemble_Behavior(L"Action_Hit", pSequence_Hit)))
		return E_FAIL;
	if (FAILED(pSelector_Alert->Assemble_Behavior(L"Sequence_Patrol", pSequence_Patrol)))
		return E_FAIL;
	if (FAILED(pSelector_Alert->Assemble_Behavior(L"Pattern_BackDash", pPattern_BackDash)))
		return E_FAIL;
	if (FAILED(pSelector_Alert->Assemble_Behavior(L"Pattern_Attack", pPattern_Attack)))
		return E_FAIL;

	pSequence_Hit->Assemble_Childs();
	pSequence_Patrol->Assemble_Childs();
	pPattern_BackDash->Assemble_Childs();
	pPattern_Attack->Assemble_Childs();

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
