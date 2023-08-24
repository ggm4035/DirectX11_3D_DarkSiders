
#include "CGoblin.h"

#include "MonoBehavior_Defines.h"
#include "CBlackBoard.h"
#include "CGameInstance.h"
#include "CPlayer.h"

#include "CAoE.h"
#include "CSpawn.h"
#include "CUI_Sprite.h"
#include "CSoul.h"

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

	m_pAttack->Set_Damage(15);
	m_pDeffence->Set_Deffence(0);
	m_pHealth->Set_Max_Hp(100);
	m_pHealth->Set_HP(100);

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
	else
	{
		m_fSpawnTimeAcc += TimeDelta;

		if (2.f < m_fSpawnTimeAcc)
			return;

		_matrix Matrix = /*XMMatrixScaling(2.f, 2.f, 2.f) * */XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, 0.1f, 0.f) * m_pTransformCom->Get_WorldMatrix();
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, Matrix);
		m_pSpawn->Tick(WorldMatrix, TimeDelta);
	}
}

void CGoblin::AfterFrustumTick(const _double& TimeDelta)
{
	if (false == m_isRender)
		return;

	CMonster::AfterFrustumTick(TimeDelta);
}

/* 여기는 콜라이더가 객체의 상태를 변경(On_Collision) */
void CGoblin::Late_Tick(const _double& TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);

	if (2.f < m_fSpawnTimeAcc)
		return;

	m_pSpawn->Late_Tick(TimeDelta);
}

HRESULT CGoblin::Render()
{
	m_iPassNum = 0;

	if (CHealth::HIT_NONE != m_pHealth->Get_Current_HitState())
		m_iPassNum = 1;

	if (FAILED(CMonster::Render()))
		return E_FAIL;

	return S_OK;
}

void CGoblin::Dead_Motion(const _double& TimeDelta)
{
	CMonster::Dead_Motion(TimeDelta);
	m_fExplosionTimeAcc += TimeDelta;

	if (1.9f > m_fExplosionTimeAcc)
	{
		_float fScale = m_fExplosionTimeAcc * 4.f;
		_matrix Matrix = XMMatrixScaling(fScale, fScale, fScale) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, 0.1f, 0.f) * m_pTransformCom->Get_WorldMatrix();
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, Matrix);
		m_pAoe->Tick(WorldMatrix);
		m_pAoe->Late_Tick(TimeDelta);
	}
	
	if (true == m_isRender && 2.f <= m_fExplosionTimeAcc)
	{
		CGameInstance::GetInstance()->Play_Sound(L"en_fleamag_explode_01.ogg", CSound_Manager::SOUND_ENEMY, 2.f, true);
		m_isRender = false;
	}

	if (true == m_isRender && 1.9f <= m_fExplosionTimeAcc)
	{
		CCollider* pCollider = Find_Collider(L"Col_Explosion");
		if (nullptr == pCollider)
			return;

		pCollider->Set_Enable(true);
	}

	if (false == m_isRender)
	{
		m_pSprite->Tick(TimeDelta);
		m_pSprite->Late_Tick(TimeDelta);

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

		if (0 == m_vecSouls.size())
			m_isRemove = true;
	}
}

void CGoblin::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (Collision.pOtherCollider->Get_Tag() == L"Col_Body" &&
		Collision.pMyCollider->Get_Tag() == L"Col_Body")
	{
		_vector vOtherPosition = Collision.pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vOtherPosition);
		m_pTransformCom->Repersive(vDir, TimeDelta);
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isSpawn = true;
		m_isRangeInPlayer = true;
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Attack" &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		Collision.pOther->Get_Damaged(m_pAttack);
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Explosion" &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		_float4 vPosition;
		XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		dynamic_cast<CPlayer*>(Collision.pOther)->Get_Damaged_Knockback(vPosition, m_pAttack);
	}
}

void CGoblin::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if ((Collision.pOtherCollider->Get_Collider_Group() == CCollider::COL_STATIC ||
		Collision.pOtherCollider->Get_Tag() == L"Col_Body") &&
		Collision.pMyCollider->Get_Tag() == L"Col_Body")
	{
		_vector vOtherPosition = Collision.pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vOtherPosition);
		m_pTransformCom->Repersive(vDir, TimeDelta);
	}

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
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_Goblin", L"Com_Model", 
		(CComponent**)&m_pModelCom, this)))
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

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Sprite::UISPRITEDESC SpriteDesc;
	SpriteDesc.SpriteDesc.bRepeat = false;
	SpriteDesc.SpriteDesc.fFrameSpeed = 0.5f;
	SpriteDesc.SpriteDesc.iNumHeight = 6;
	SpriteDesc.SpriteDesc.iNumWidth = 6;
	SpriteDesc.m_fX = 0.f;
	SpriteDesc.m_fY = 0.f;
	SpriteDesc.m_fDepth = 0.f;
	SpriteDesc.m_fSizeX = 60.f;
	SpriteDesc.m_fSizeY = 60.f;
	SpriteDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	SpriteDesc.vOffset = _float3(0.f, 1.f, 0.f);
	SpriteDesc.iTextureLevel = LEVEL_GAMEPLAY;
	SpriteDesc.wstrTextureTag = L"Texture_Explosion";

	m_pSprite = dynamic_cast<CUI_Sprite*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"UI_Sprite", L"UI_Sprite", this, &SpriteDesc));
	if (nullptr == m_pSprite)
		return E_FAIL;

	CAoE::AOEDESC AoeDesc;
	AoeDesc.wstrTextureTag = L"Texture_UI_GenericCircle";
	AoeDesc.iTextureLevel = LEVEL_GAMEPLAY;
	AoeDesc.iPassNum = 4;
	AoeDesc.strShaderResourceTag = "g_TextureCircle";

	m_pAoe = dynamic_cast<CAoE*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"AOE", L"Explosion", this, &AoeDesc));
	if (nullptr == m_pAoe)
		return E_FAIL;

	m_pSpawn = dynamic_cast<CSpawn*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"Spawn", L"Spawn", this, nullptr));
	if (nullptr == m_pSpawn)
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
	CSelector* pAttack = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Selector", this));
	if (nullptr == pAttack)
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

	pHit->Bind_SoundTag(L"en_fleamag_impact_01.ogg");
	pAction_Spawn->Bind_AnimationTag("Spawn");
	pAction_Spawn->Just_One_Time_Action();
	pAction_Explosion_Start->Bind_AnimationTag("Explosion_Start");
	pAction_Explosion_Run->Bind_AnimationTag("Explosion_Run");
	pAction_Explosion->Bind_AnimationTag("Explosion_Dead");
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
			CHealth* pHealth = { nullptr };
			pBlackBoard->Get_Type(L"pHealth", pHealth);
			if (nullptr == pHealth)
				return false;

			return pHealth->isDead();
		});

	pAction_Explosion_Start->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsRender = { nullptr };
			pBlackBoard->Get_Type(L"isRender", pIsRender);
			if (nullptr == pIsRender)
				return false;

			return *pIsRender;
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
	if (FAILED(pSelector->Assemble_Behavior(L"Attack", pAttack)))
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

	if (FAILED(Make_Attack(pAttack)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CGoblin::Make_Attack(CSelector* pSelector)
{
	if (nullptr == pSelector)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSequence* pSequence_Attack = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Sequence", this));
	if (nullptr == pSequence_Attack)
		return E_FAIL;
	CRandomSelector* pRandom_Attack = dynamic_cast<CRandomSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"RandomSelector", this));
	if (nullptr == pRandom_Attack)
		return E_FAIL;

	/* Action */
	CAction* pAction_Follow = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Follow)
		return E_FAIL;
	CAction* pAction_LookAt = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_LookAt)
		return E_FAIL;

	CAction* pAction_Attack1 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Attack1)
		return E_FAIL;
	CAction* pAction_Attack2 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Action", this));
	if (nullptr == pAction_Attack2)
		return E_FAIL;

	/* Movement */
	CFollow* pFollow = dynamic_cast<CFollow*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_Follow", this));
	if (nullptr == pFollow)
		return E_FAIL;
	CLookAtTarget* pLookAtTarget = dynamic_cast<CLookAtTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"Tsk_LookAtTarget", this));
	if (nullptr == pLookAtTarget)
		return E_FAIL;

	pSelector->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			CHealth* pHealth = { nullptr };
			pBlackBoard->Get_Type(L"pHealth", pHealth);
			if (nullptr == pHealth)
				return false;

			if (CHealth::HIT_NONE != pHealth->Get_Current_HitState())
				return false;

			return true;
		});

	pSelector->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsSpawn = { nullptr };
			pBlackBoard->Get_Type(L"isSpawn", pIsSpawn);
			if (nullptr == pIsSpawn)
				return false;

			return *pIsSpawn;
		});

	pAction_Follow->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			if (true == m_pModelCom->isLoopAnimation() ||
				true == m_pModelCom->isFinishedAnimation())
				return true;

			return false;
		});

	pAction_Follow->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsAbleAttack = { nullptr };

			pBlackBoard->Get_Type(L"isAbleAttack", pIsAbleAttack);
			if (nullptr == pIsAbleAttack)
				return false;

			return !(*pIsAbleAttack);
		});

	pAction_Follow->Bind_AnimationTag("Run");
	pAction_LookAt->Bind_AnimationTag("Run");

	pFollow->Bind_Move_Speed(0.9f);
	pFollow->Bind_Turn_Speed(5.f);
	pLookAtTarget->Set_Timer(1.5f);

	pAction_Attack1->Bind_AnimationTag("Attack_1");
	pAction_Attack2->Bind_AnimationTag("Attack_2");

	if (FAILED(pSelector->Assemble_Behavior(L"Action_Follow", pAction_Follow)))
		return E_FAIL;
	if (FAILED(pSelector->Assemble_Behavior(L"Sequence_Attack", pSequence_Attack)))
		return E_FAIL;

	if (FAILED(pAction_Follow->Assemble_Behavior(L"Tsk_Follow", pFollow)))
		return E_FAIL;
	if (FAILED(pSequence_Attack->Assemble_Behavior(L"Action_LookAt", pAction_LookAt)))
		return E_FAIL;
	if (FAILED(pSequence_Attack->Assemble_Behavior(L"Random_Attack", pRandom_Attack)))
		return E_FAIL;

	if (FAILED(pAction_LookAt->Assemble_Behavior(L"Tsk_LookAtTarget", pLookAtTarget)))
		return E_FAIL;
	if (FAILED(pRandom_Attack->Assemble_Behavior(pGameInstance->strToWStr("Attack_1"), pAction_Attack1)))
		return E_FAIL;
	if (FAILED(pRandom_Attack->Assemble_Behavior(pGameInstance->strToWStr("Attack_2"), pAction_Attack2)))
		return E_FAIL;

	Safe_Release(pGameInstance);
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
	Safe_Release(m_pSpawn);
	Safe_Release(m_pSprite);

	CMonster::Free();
}
