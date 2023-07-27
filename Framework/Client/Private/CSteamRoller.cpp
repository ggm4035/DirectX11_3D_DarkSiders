#include "CSteamRoller.h"

#include "MonoBehavior_Defines.h"
#include "CBlackBoard.h"
#include "CGameInstance.h"
#include "CPlayer.h"

CSteamRoller::CSteamRoller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CSteamRoller::CSteamRoller(const CSteamRoller& rhs)
	: CMonster(rhs)
{
}

HRESULT CSteamRoller::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CMonster::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4(&m_vResponPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_Status.iHP = 30;
	m_Status.iMaxHP = 30;

	return S_OK;
}

void CSteamRoller::Tick(const _double& TimeDelta)
{
	CMonster::Tick(TimeDelta);
}

void CSteamRoller::AfterFrustumTick(const _double& TimeDelta)
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

#ifdef _DEBUG
		if (true == m_isRender && FAILED(Add_Colliders_Debug_Render_Group(m_pRendererCom)))
			return;
#endif
	}

	Safe_Release(pGameInstance);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CSteamRoller::Late_Tick(const _double& TimeDelta)
{
	On_Colisions(TimeDelta);
}

HRESULT CSteamRoller::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CSteamRoller::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);

	if (Collision.pMyCollider->Get_Tag() == L"Col_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isSpawn = true;
	}
}

void CSteamRoller::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
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

void CSteamRoller::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
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

HRESULT CSteamRoller::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_SteamRoller",
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	CBounding_Sphere::SPHEREDESC SphereDesc;
	CBounding_AABB::AABBDESC AABBDesc;

	/* Col_Body */
	AABBDesc.vExtents = _float3(2.f, 2.f, 2.f);
	AABBDesc.eGroup = CCollider::COL_ENEMY;
	AABBDesc.vOffset = _float3(0.f, 2.f, 0.f);
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Col_Body", &AABBDesc)))
		return E_FAIL;

	/* Col_Range */
	SphereDesc.fRadius = 20.f;
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

/* 보스임 */
HRESULT CSteamRoller::Make_AI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;

	CAction* pAction_Detect = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Detect)
		return E_FAIL;
	CAction_Hit* pSequence_Hit = dynamic_cast<CAction_Hit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence_Hit", this));
	if (nullptr == pSequence_Hit)
		return E_FAIL;
	CPattern_Roll* pPattern_Roll = dynamic_cast<CPattern_Roll*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Pattern_Roll", this));
	if (nullptr == pPattern_Roll)
		return E_FAIL;
	CPattern_Attack* pPattern_Attack = dynamic_cast<CPattern_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Pattern_Attack", this));
	if (nullptr == pPattern_Attack)
		return E_FAIL;

	pAction_Detect->Bind_AnimationTag("Enrage");
	pAction_Detect->Just_One_Time_Action();

	pPattern_Attack->Bind_FollowAnimTag("Walk_F");
	pPattern_Attack->Add_Attack_AnimTag("Attack_1");
	pPattern_Attack->Add_Attack_AnimTag("Attack_2");
	pPattern_Attack->Add_Attack_AnimTag("Attack_3");

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Action_Detect", pAction_Detect)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Pattern_Roll", pPattern_Roll)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Hit", pSequence_Hit)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Pattern_Attack", pPattern_Attack)))
		return E_FAIL;

	if (FAILED(pPattern_Roll->Assemble_Childs()))
		return E_FAIL;
	if (FAILED(pSequence_Hit->Assemble_Childs()))
		return E_FAIL;
	if (FAILED(pPattern_Attack->Assemble_Childs()))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CSteamRoller* CSteamRoller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSteamRoller* pInstance = new CSteamRoller(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTusker");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CSteamRoller* CSteamRoller::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CSteamRoller* pInstance = new CSteamRoller(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CTusker");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSteamRoller::Free()
{
	CMonster::Free();
}
