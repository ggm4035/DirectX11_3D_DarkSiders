#include "stdafx.h"
#include "CLegion_Melee.h"

#include "CBlackBoard.h"
#include "MonoBehavior_Defines.h"
#include "CGameInstance.h"
#include "CPlayer.h"

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

	return S_OK;
}

void CLegion_Melee::Tick(const _double& TimeDelta)
{
	m_pTransformCom->Animation_Movement(m_pModelCom, TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Down(DIK_4))
		m_pModelCom->Change_Animation("Knockback");

	Safe_Release(pGameInstance);

	m_pRoot->Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta, m_pNavigationCom);

	Tick_Colliders(m_pTransformCom->Get_WorldMatrix());
}

void CLegion_Melee::AfterFrustumTick(const _double& TimeDelta)
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

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}

	Safe_Release(pGameInstance);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CLegion_Melee::Late_Tick(const _double& TimeDelta)
{
	On_Colisions(TimeDelta);
}

HRESULT CLegion_Melee::Render()
{
	if (FAILED(CMonster::Render(0)))
		return E_FAIL;

#ifdef _DEBUG

	if (FAILED(Render_Colliders()))
		return E_FAIL;

#endif

	return S_OK;
}

void CLegion_Melee::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);

	CCollider* pColRange = Find_Collider(L"Col_Range");
	if (nullptr == pColRange)
		return;

	if (Collision.pMyCollider == pColRange &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isRangeInPlayer = true;
		m_isSpawn = true;
	}
}

void CLegion_Melee::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CCollider* pColRange = Find_Collider(L"Col_Range");
	if (nullptr == pColRange)
		return;

	CMonster::OnCollisionStay(Collision, TimeDelta);
	if (Collision.pMyCollider == pColRange &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isRangeInPlayer = true;
	}
}

void CLegion_Melee::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CCollider* pColRange = Find_Collider(L"Col_Range");
	if (nullptr == pColRange)
		return;

	if (Collision.pMyCollider == pColRange &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isRangeInPlayer = false;
	}
}

HRESULT CLegion_Melee::Add_Components()
{
	if (FAILED(CMonster::Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_Legion",
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

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

	CBounding_AABB::AABBDESC AABBDesc;
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

HRESULT CLegion_Melee::Make_AI()
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
	m_pRoot->Add_Type(L"ePreHitState", &m_ePreHitState);
	m_pRoot->Add_Type(L"isRangeInPlayer", &m_isRangeInPlayer);
	m_pRoot->Add_Type(L"isSpawn", &m_isSpawn);
	m_pRoot->Add_Type(L"isSpawnEnd", &m_isSpawnEnd);
	m_pRoot->Add_Type(L"pTarget", pGameInstance->Get_Player());
	m_pRoot->Add_Type(L"pModel", m_pModelCom);

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;

	/*CSpawn* pSpawn = dynamic_cast<CSpawn*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Spawn", this));
	if (nullptr == pSpawn)
		return E_FAIL;*/

	CHit* pHit = dynamic_cast<CHit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Hit", this));
	if (nullptr == pHit)
		return E_FAIL;

	CMonster_Attacks* pAttacks = dynamic_cast<CMonster_Attacks*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector_Attacks", this));
	if (nullptr == pAttacks)
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

	/*if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Spawn", pSpawn)))
		return E_FAIL;*/

	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Hit", pHit)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Selector_Attacks", pAttacks)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Wait", pWait)))
		return E_FAIL;

	if (FAILED(pAttacks->Assemble_Childs()))
		return E_FAIL;

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
	CMonster::Free();
}
