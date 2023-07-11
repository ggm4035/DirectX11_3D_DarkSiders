#include "stdafx.h"
#include "CHellHound.h"

#include "MonoBehavior_Defines.h"
#include "CGameInstance.h"
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

	return S_OK;
}

void CHellHound::Tick(const _double& TimeDelta)
{
	m_pTransformCom->Animation_Movement(m_pModelCom, TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Down(DIK_4))
		m_pModelCom->Change_Animation("Knockback");

	Safe_Release(pGameInstance);

	m_pRoot->Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pColBody)
		m_pColBody->Tick(m_pTransformCom->Get_WorldMatrix());
	if (nullptr != m_pColRange)
		m_pColRange->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CHellHound::AfterFrustumTick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		pGameInstance->Add_Collider(COL_ENEMY, m_pColBody);
		pGameInstance->Add_Collider(COL_ENEMYRANGE, m_pColRange);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}

	Safe_Release(pGameInstance);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CHellHound::Late_Tick(const _double& TimeDelta)
{
	m_pColBody->On_Collision(this, TimeDelta);
	m_pColRange->On_Collision(this, TimeDelta);
}

HRESULT CHellHound::Render()
{
	if (FAILED(CMonster::Render(0)))
		return E_FAIL;

#ifdef _DEBUG

	if (nullptr != m_pColBody)
		m_pColBody->Render();
	if (nullptr != m_pColRange)
		m_pColRange->Render();

#endif

	return S_OK;
}

void CHellHound::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);

	if (nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isRangeInPlayer = true;
	}
}

void CHellHound::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionStay(Collision, TimeDelta);
	if (nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isRangeInPlayer = true;
	}
}

void CHellHound::OnCollisionExit(const _double& TimeDelta)
{
	m_isRangeInPlayer = false;
}

HRESULT CHellHound::Add_Components()
{
	if (FAILED(CMonster::Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_HellHound",
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	CBounding_Sphere::SPHEREDESC SphereDesc;
	SphereDesc.fRadius = 1.f;
	SphereDesc.vPosition = _float3(0.f, 1.f, 0.f);

	if (FAILED(Add_Component(LEVEL_STATIC, L"Collider_Sphere", L"Col_Body",
		(CComponent**)&m_pColBody, this, &SphereDesc)))
		return E_FAIL;

	SphereDesc.fRadius = 10.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(Add_Component(LEVEL_STATIC, L"Collider_Sphere", L"Col_Range",
		(CComponent**)&m_pColRange, this, &SphereDesc)))
		return E_FAIL;

	if (FAILED(Make_AI()))
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
	m_pRoot->Add_Type(L"eCurHitState", &m_eCurHitState);
	m_pRoot->Add_Type(L"ePreHitState", &m_ePreHitState);
	m_pRoot->Add_Type(L"isRangeInPlayer", &m_isRangeInPlayer);
	m_pRoot->Add_Type(L"isSpawn", &m_isSpawn);
	m_pRoot->Add_Type(L"isSpawnEnd", &m_isSpawnEnd);
	m_pRoot->Add_Type(L"pTarget", pGameInstance->Get_Player());

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;

	CSpawn* pSpawn = dynamic_cast<CSpawn*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Spawn", this));
	if (nullptr == pSpawn)
		return E_FAIL;

	CHit* pHit = dynamic_cast<CHit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Hit", this));
	if (nullptr == pHit)
		return E_FAIL;

	CMonster_Patrol* pPatrol = dynamic_cast<CMonster_Patrol*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence_Patrol", this));
	if (nullptr == pPatrol)
		return E_FAIL;

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Spawn", pSpawn)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Hit", pHit)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Patrol", pPatrol)))
		return E_FAIL;

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
	Safe_Release(m_pColBody);
	Safe_Release(m_pColRange);

	CMonster::Free();
}
