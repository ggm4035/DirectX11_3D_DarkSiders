
#include "CHellHound.h"

#include "MonoBehavior_Defines.h"
#include "CGameInstance.h"
#include "CBlackBoard.h"
#include "CPlayer.h"
#include "CSoul.h"

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

	pHit->Bind_SoundTag(L"en_hellhound_impact_vo_02.ogg");
	pPattern_Attack->Set_CoolTime(1.f);
	pPattern_Attack->Bind_FollowAnimTag("Run");
	pPattern_Attack->Add_Attack_AnimTag("Attack_1");
	pPattern_Attack->Add_Attack_AnimTag("Attack_2");

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
	for (auto& pSoul : m_vecSouls)
		Safe_Release(pSoul);

	CMonster::Free();
}
