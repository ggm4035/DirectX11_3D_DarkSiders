
#include "CGoblin.h"

#include "MonoBehavior_Defines.h"
#include "CBlackBoard.h"
#include "CGameInstance.h"
#include "CPlayer.h"

#include "CAoE.h"

CGoblin::CGoblin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CGoblin::CGoblin(const CGoblin& rhs)
	: CMonster(rhs)
{
}

HRESULT CGoblin::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CMonster::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4(&m_vResponPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_Status.iHP = 4;
	m_Status.iMaxHP = 4;

	return S_OK;
}

void CGoblin::Tick(const _double& TimeDelta)
{
	CMonster::Tick(TimeDelta);

	if (false == m_isSpawn)
	{
		_vector vPosition = XMLoadFloat4(&m_vResponPosition);

		vPosition.m128_f32[1] = vPosition.m128_f32[1] - 2.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

void CGoblin::AfterFrustumTick(const _double& TimeDelta)
{
	CMonster::AfterFrustumTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, m_pAoe);
	}

	Safe_Release(pGameInstance);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CGoblin::Late_Tick(const _double& TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);
}

HRESULT CGoblin::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CGoblin::Dead_Motion(const _double& TimeDelta)
{
	CMonster::Dead_Motion(TimeDelta);
	m_fExplosionTimeAcc += TimeDelta;

	_float fScale = m_fExplosionTimeAcc * 4.f;
	_matrix Matrix = XMMatrixScaling(fScale, fScale, fScale) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, 0.1f, 0.f) * m_pTransformCom->Get_WorldMatrix();
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, Matrix);

	m_pAoe->Tick(WorldMatrix);

	if (1.9f <= m_fExplosionTimeAcc)
	{
		CCollider* pCollider = Find_Collider(L"Col_Explosion");
		if (nullptr == pCollider)
			return;

		pCollider->Set_Enable(true);
	}

	if (2.f <= m_fExplosionTimeAcc)
	{
		m_isRemove = true;
	}
}

void CGoblin::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	CMonster::OnCollisionEnter(Collision, TimeDelta);

	if (Collision.pMyCollider->Get_Tag() == L"Col_Attack"&&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		Collision.pOther->Get_Damaged();
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Explosion" &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		_float4 vPosition;
		XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		Collision.pOther->Get_Damaged_Knockback(vPosition);
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isSpawn = true;
	}
}

void CGoblin::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
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

void CGoblin::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
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

HRESULT CGoblin::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_Goblin",
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	CBounding_Sphere::SPHEREDESC SphereDesc;
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

	/* Col_Attack */
	SphereDesc.fRadius = 1.f;
	SphereDesc.eGroup = CCollider::COL_ENEMY_ATK;
	SphereDesc.vOffset = _float3(0.f, 0.f, 1.f);
	SphereDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Attack", &SphereDesc)))
		return E_FAIL;

	/* Col_Explosion */
	SphereDesc.fRadius = 4.f;
	SphereDesc.eGroup = CCollider::COL_ENEMY_ATK;
	SphereDesc.vOffset = _float3(0.f, 0.f, 0.f);
	SphereDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Explosion", &SphereDesc)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CAoE::AOEDESC AoeDesc;

	AoeDesc.wstrTextureTag = L"Texture_UI_GenericCircle";
	AoeDesc.iTextureLevel = LEVEL_GAMEPLAY;
	AoeDesc.iPassNum = 4;
	AoeDesc.strShaderResourceTag = "g_TextureCircle";

	m_pAoe = dynamic_cast<CAoE*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"AOE", L"Explosion", this, &AoeDesc));
	if (nullptr == m_pAoe)
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

/* 평소에는 근접공격, 일정 체력 이하로 떨어지면 자폭돌진 */
HRESULT CGoblin::Make_AI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Behaviors */
	CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pSelector)
		return E_FAIL;

	CSequence* pSequence_Explosion = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence", this));
	if (nullptr == pSequence_Explosion)
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
	CHit* pHit = dynamic_cast<CHit*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Hit", this));
	if (nullptr == pHit)
		return E_FAIL;
	CAction* pAction_Explosion_Start = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Explosion_Start)
		return E_FAIL;
	CAction* pAction_Explosion_Run = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Explosion_Run)
		return E_FAIL;
	CAction* pAction_Explosion = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Explosion)
		return E_FAIL;
	CPattern_Attack* pPattern_Attack = dynamic_cast<CPattern_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Pattern_Attack", this));
	if (nullptr == pPattern_Attack)
		return E_FAIL;

	CJump* pJump = dynamic_cast<CJump*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Jump", this));
	if (nullptr == pJump)
		return E_FAIL;
	CMove* pMove = dynamic_cast<CMove*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Move", this));
	if (nullptr == pMove)
		return E_FAIL;
	CFollow* pFollow = dynamic_cast<CFollow*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Follow", this));
	if (nullptr == pFollow)
		return E_FAIL;
	CDead* pDead = dynamic_cast<CDead*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Dead", this));
	if (nullptr == pDead)
		return E_FAIL;

	pAction_Spawn->Bind_AnimationTag("Spawn");
	pAction_Spawn->Just_One_Time_Action();
	pAction_Explosion_Start->Bind_AnimationTag("Explosion_Start");
	pAction_Explosion_Run->Bind_AnimationTag("Explosion_Run");
	pAction_Explosion->Bind_AnimationTag("Explosion_Dead");
	pPattern_Attack->Bind_FollowAnimTag("Run");
	pPattern_Attack->Add_Attack_AnimTag("Attack_1");
	pPattern_Attack->Add_Attack_AnimTag("Attack_2");
	pJump->Bind_Jump_Force(0.7f);
	pMove->Bind_Move_Force(0.5f);
	pFollow->Set_Timer(5.f);
	pFollow->Bind_Move_Speed(1.5f);

	pSequence_Spawn->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsSpawn = { nullptr };
			pBlackBoard->Get_Type(L"isSpawn", pIsSpawn);
			if (nullptr == pIsSpawn)
				return false;

			return *pIsSpawn;
		});

	pAction_Explosion_Start->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsDead = { nullptr };
			pBlackBoard->Get_Type(L"isDead", pIsDead);
			if (nullptr == pIsDead)
				return false;

			return *pIsDead;
		});

	/* Assemble */
	if (FAILED(m_pRoot->Assemble_Behavior(L"Selector", pSelector)))
		return E_FAIL;

	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Spawn", pSequence_Spawn)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Tsk_Hit", pHit)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Explosion", pSequence_Explosion)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Pattern_Attack", pPattern_Attack)))
		return E_FAIL;

	if (FAILED(pSequence_Spawn->Assemble_Behavior(L"Tsk_RandomLook", pRandomLook)))
		return E_FAIL;
	if (FAILED(pSequence_Spawn->Assemble_Behavior(L"Action_Spawn", pAction_Spawn)))
		return E_FAIL;
	if (FAILED(pSequence_Explosion->Assemble_Behavior(L"Action_Explosion_Start", pAction_Explosion_Start)))
		return E_FAIL;
	if (FAILED(pSequence_Explosion->Assemble_Behavior(L"Action_Explosion_Run", pAction_Explosion_Run)))
		return E_FAIL;
	if (FAILED(pSequence_Explosion->Assemble_Behavior(L"Action_Explosion", pAction_Explosion)))
		return E_FAIL;

	if (FAILED(pAction_Spawn->Assemble_Behavior(L"Tsk_Jump", pJump)))
		return E_FAIL;
	if (FAILED(pAction_Spawn->Assemble_Behavior(L"Tsk_Move", pMove)))
		return E_FAIL;
	if (FAILED(pAction_Explosion_Run->Assemble_Behavior(L"Tsk_Follow", pFollow)))
		return E_FAIL;
	if (FAILED(pAction_Explosion->Assemble_Behavior(L"Tsk_Dead", pDead)))
		return E_FAIL;

	if (FAILED(pPattern_Attack->Assemble_Childs()))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CGoblin* CGoblin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin* pInstance = new CGoblin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CGoblin");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGoblin* CGoblin::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CGoblin* pInstance = new CGoblin(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CGoblin");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGoblin::Free()
{
	Safe_Release(m_pAoe);

	CMonster::Free();
}
