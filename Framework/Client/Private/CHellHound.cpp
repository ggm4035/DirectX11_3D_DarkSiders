
#include "CHellHound.h"

#include "MonoBehavior_Defines.h"
#include "CGameInstance.h"
#include "CBlackBoard.h"
#include "CPlayer.h"
#include "CSoul.h"
#include "CSpawn.h"

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

	m_pAttack->Set_Damage(15);
	m_pDeffence->Set_Deffence(0);
	m_pHealth->Set_Max_Hp(100);
	m_pHealth->Set_HP(100);

	return S_OK;
}

void CHellHound::Tick(const _double& TimeDelta)
{
	m_fSpawnTimeAcc += TimeDelta;

	CMonster::Tick(TimeDelta);

	if (2.f > m_fSpawnTimeAcc)
	{
		_matrix Matrix = XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, 0.1f, 0.f) * m_pTransformCom->Get_WorldMatrix();
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, Matrix);
		m_pSpawn->Tick(WorldMatrix, TimeDelta);
	}
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
	m_iPassNum = 0;

	if (CHealth::HIT_NONE != m_pHealth->Get_Current_HitState())
		m_iPassNum = 1;
	if (true == m_pHealth->isDead())
		m_iPassNum = 3;

	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CHellHound::Dead_Motion(const _double& TimeDelta)
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

void CHellHound::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);
}

void CHellHound::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionStay(Collision, TimeDelta);
}

void CHellHound::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionExit(Collision, TimeDelta);
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

	if (true == m_isSpawn)
	{
		if (FAILED(Make_SpawnAI()))
			return E_FAIL;
	}

	else if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHellHound::Make_AI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;
	CSelector* pSelector_Pattern = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector_Pattern)
		return E_FAIL;

	CAction_Hit* pHit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Action_Hit", this));
	if (nullptr == pHit)
		return E_FAIL;
	CPattern_Patrol* pPatrol = dynamic_cast<CPattern_Patrol*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence_Patrol", this));
	if (nullptr == pPatrol)
		return E_FAIL;
	CPattern_Attack* pPattern_Attack = dynamic_cast<CPattern_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Pattern_Attack", this));
	if (nullptr == pPattern_Attack)
		return E_FAIL;

	pSelector_Pattern->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CHealth* pHealth = { nullptr };
			if (FAILED(pBlackBoard->Get_Type(L"pHealth", pHealth)))
				return false;

			return !pHealth->isDead();
		});

	CAction::SOUNDDESC SoundDesc;
	pHit->Bind_SoundTag(L"en_hellhound_impact_vo_02.ogg");
	pPattern_Attack->Set_CoolTime(1.f);
	pPattern_Attack->Bind_FollowAnimTag("Run");
	CPattern_Attack::ATTACKDESC Desc;
	Desc.strAttackAnimTag = "Attack_1";
	SoundDesc.isPlaySound = false;
	SoundDesc.eChennel = CSound_Manager::SOUND_EFFECT_2;
	SoundDesc.fTime = 0.01f;
	SoundDesc.fVolum = 0.1f;
	SoundDesc.wstrSoundTag = L"en_hellhound_atk_bite_02.ogg";
	Desc.Sounds.push_back(SoundDesc);
	pPattern_Attack->Add_Attack(Desc);

	Desc.strAttackAnimTag = "Attack_2";
	SoundDesc.fVolum = 0.5f;
	SoundDesc.wstrSoundTag = L"en_hellhound_atk_bite_run_02.ogg";
	pPattern_Attack->Add_Attack(Desc);

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Action_Hit", pHit)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Selector_Pattern", pSelector_Pattern)))
		return E_FAIL;

	if (FAILED(pSelector_Pattern->Assemble_Behavior(L"Sequence_Patrol", pPatrol)))
		return E_FAIL;
	if (FAILED(pSelector_Pattern->Assemble_Behavior(L"Pattern_Attack", pPattern_Attack)))
		return E_FAIL;

	if (FAILED(pHit->Assemble_Childs()))
		return E_FAIL;
	if (FAILED(pPattern_Attack->Assemble_Childs()))
		return E_FAIL;
	if (FAILED(pPatrol->Assemble_Childs()))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CHellHound::Make_SpawnAI()
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

	CRandomLook* pRandomLook = dynamic_cast<CRandomLook*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_RandomLook", this));
	if (nullptr == pRandomLook)
		return E_FAIL;
	CAction* pAction_Spawn = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Spawn)
		return E_FAIL;
	CAction_Hit* pSequence_Hit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Action_Hit", this));
	if (nullptr == pSequence_Hit)
		return E_FAIL;
	CPattern_FollowAtk* pAttack = dynamic_cast<CPattern_FollowAtk*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Pattern_FollowAtk", this));
	if (nullptr == pAttack)
		return E_FAIL;

	CJump* pJump = dynamic_cast<CJump*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Jump", this));
	if (nullptr == pJump)
		return E_FAIL;
	CMove* pMove = dynamic_cast<CMove*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Move", this));
	if (nullptr == pMove)
		return E_FAIL;

	pSequence_Hit->Bind_SoundTag(L"en_fleamag_armored_block_01.ogg");
	CAction::SOUNDDESC SoundDesc;
	pAction_Spawn->Bind_AnimationTag("Spawn");
	pAction_Spawn->Just_One_Time_Action();
	SoundDesc.isPlaySound = false;
	SoundDesc.eChennel = CSound_Manager::SOUND_EFFECT_2;
	SoundDesc.fVolum = 0.1f;
	SoundDesc.fTime = 0.01f;
	SoundDesc.wstrSoundTag = L"en_fleamag_spawn_04.ogg";
	pAction_Spawn->Add_Sound(SoundDesc);
	CPattern_FollowAtk::ATTACKDESC AttackDesc;
	SoundDesc.isPlaySound = false;
	SoundDesc.eChennel = CSound_Manager::SOUND_EFFECT_2;
	SoundDesc.fTime = 0.01f;
	SoundDesc.fVolum = 0.1f;
	SoundDesc.wstrSoundTag = L"en_fleamag_scream_long_01.ogg";
	AttackDesc.Sounds.push_back(SoundDesc);
	AttackDesc.strAttackAnimTag = "Attack_1";
	pAttack->Add_Attack(AttackDesc);

	AttackDesc.Sounds.clear();
	SoundDesc.wstrSoundTag = L"en_fleamag_scream_long_02.ogg";
	AttackDesc.Sounds.push_back(SoundDesc);
	AttackDesc.strAttackAnimTag = "Attack_2";
	pAttack->Add_Attack(AttackDesc);

	pJump->Bind_Jump_Force(0.7f);
	pMove->Bind_Move_Force(0.5f);

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Spawn", pSequence_Spawn)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Action_Hit", pSequence_Hit)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Attack", pAttack)))
		return E_FAIL;

	if (FAILED(pSequence_Spawn->Assemble_Behavior(L"Tsk_RandomLook", pRandomLook)))
		return E_FAIL;
	if (FAILED(pSequence_Spawn->Assemble_Behavior(L"Action_Spawn", pAction_Spawn)))
		return E_FAIL;

	if (FAILED(pAction_Spawn->Assemble_Behavior(L"Tsk_Jump", pJump)))
		return E_FAIL;
	if (FAILED(pAction_Spawn->Assemble_Behavior(L"Tsk_Move", pMove)))
		return E_FAIL;

	if (FAILED(pSequence_Hit->Assemble_Childs()))
		return E_FAIL;
	if (FAILED(pAttack->Assemble_Childs()))
		return E_FAIL;

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
