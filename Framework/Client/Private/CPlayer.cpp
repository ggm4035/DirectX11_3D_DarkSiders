
#include "CPlayer.h"

#include "CGameInstance.h"

#include "CPlayerAction.h"
#include "CWeapon.h"
#include "CSwordTrail.h"
#include "CRoot.h"

#include "CQuake_Effect.h"
#include "CCamera_Free.h"
#include "CInven.h"

void CPlayer::Get_Damaged(const CAttack* pAttack)
{
	_int iDeffence = m_pDeffence->Get_Deffence();
	_int iDamage = pAttack->Get_Damage();

	if(false == pAttack->isIgnoreDeffence())
		Saturate(iDamage -= iDeffence, 0, iDamage);

	m_pHealth->Damaged(iDamage);
}

void CPlayer::Get_Damaged_Knockback(const _float4& _vPosition, const CAttack* pAttack)
{
	_vector vPosition = XMLoadFloat4(&_vPosition);
	m_pTransformCom->LookAt(vPosition);
	m_pActionCom->Set_State(CPlayerAction::STATE_KNOCKBACK);

	_int iDeffence = m_pDeffence->Get_Deffence();
	_int iDamage = pAttack->Get_Damage();

	if (false == pAttack->isIgnoreDeffence())
		Saturate(iDamage -= iDeffence, 0, iDamage);

	m_pHealth->Damaged_By_Knockback(iDamage);
}

CCollider* CPlayer::Get_Collider(const wstring& wstrColliderTag)
{
	auto iter = m_umapColliders.find(wstrColliderTag);

	if (iter == m_umapColliders.end())
		return nullptr;

	return iter->second;
}

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_pTransformCom->Set_Matrix(reinterpret_cast<PLAYERDESC*>(pArg)->WorldMatrix);
		m_pTransformCom->Set_Angle(reinterpret_cast<PLAYERDESC*>(pArg)->vAngle);
	}

	if (FAILED(m_pRoot->Assemble_Behavior(L"PlayerAction", m_pActionCom)))
		return E_FAIL;

	if (FAILED(m_pActionCom->AssembleBehaviors()))
		return E_FAIL;

	/* Setup Notify */
	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	CGameInstance::GetInstance()->Set_Player(this);

	return S_OK;
}

void CPlayer::Tick(const _double& TimeDelta)
{
	m_fTimeAcc += TimeDelta;

	if (CGameInstance::GetInstance()->Key_Down(DIK_F3))
		dynamic_cast<CCamera_Free*>(Get_Parts(L"Camera_Free"))->Set_CamState(CCamera_Free::CAM_FREE);

	if (true == m_isRenderZoom)
	{
		Set_ZommBlur(TimeDelta);
	}

	if (CGameInstance::GetInstance()->Key_Down(DIK_I))
		m_pInven->ToggleSwitch();

	CGameObject3D::Tick(TimeDelta);
	
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//cout << vPos.m128_f32[0] << ", " << vPos.m128_f32[1] << ", " << vPos.m128_f32[2] << endl;
	m_pActionCom->Tick(TimeDelta);

	m_pTransformCom->Animation_Movement(m_pModelCom, TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta, m_pNavigationCom);
	
	m_pInven->Tick(TimeDelta);
	Tick_Colliders(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::AfterFrustumTick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* �ݶ��̴� �Ŵ����� �ݶ��̴� �߰� */
	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0))
	{
		if (FAILED(Add_Colliders_To_Manager()))
		{
			MSG_BOX("Failed to Add Colliders To Manager");
			Safe_Release(pGameInstance);
			return;
		}
		
		for (auto Pair : m_Parts)
			Pair.second->AfterFrustumTick(TimeDelta);

		m_pInven->AfterFrustumTick(TimeDelta);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

#ifdef _DEBUG
		if (true == m_isRender && FAILED(Add_Colliders_Debug_Render_Group(m_pRendererCom)))
			return;
#endif
	}

	Safe_Release(pGameInstance);
}

/* ���� ���� �ν��Ͻ����� �ݶ��̴����� �浹�� ���� */

void CPlayer::Late_Tick(const _double& TimeDelta)
{
	CGameObject3D::Late_Tick(TimeDelta);

	m_pInven->Late_Tick(TimeDelta);

	/* �浹 ���� �۾��� ���� (�޽��� ������ ó���ϴ� ������) */

	On_Colisions(TimeDelta);
}

/* ��� ���ӿ�����Ʈ�� latetick�� ������ ���� �ν��Ͻ����� �ݶ��̴��� ��� ����Ʈ�� ��� */

HRESULT CPlayer::Render()
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	_uint iPassNum = { 0 };

	if (CHealth::HIT_NONE != m_pHealth->Get_Current_HitState())
		iPassNum = 1;
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TYPE_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TYPE_NORMALS);

		if (FAILED(m_pShaderCom->Begin(iPassNum)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CPlayer::Render_Shadow()
{
	if (FAILED(Set_Shader_Shadow_Resources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CPlayer::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if ((Collision.pMyCollider->Get_Tag() == L"Col_Attack" ||
		Collision.pMyCollider->Get_Tag() == L"Col_WheelWind") &&
		(Collision.pOtherCollider->Get_Tag() == L"Col_Body" ||
			Collision.pOtherCollider->Get_Collider_Group() == CCollider::COL_BOSS ||
			Collision.pOtherCollider->Get_Collider_Group() == CCollider::COL_STATIC))
	{
		if(Collision.pMyCollider->Get_Tag() != L"Col_WheelWind")
			m_isRenderZoom = true;

		if(CPlayerAction::STATE_LEAP != m_pActionCom->Get_State())
			Collision.pOther->Get_Damaged(m_pAttack);
		else
			Collision.pOther->Get_Skill_Damaged(m_pAttack);
	}
}

void CPlayer::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if ((Collision.pOtherCollider->Get_Collider_Group() == CCollider::COL_STATIC || 
		Collision.pOtherCollider->Get_Tag() == L"Col_Attack_Roll") &&
		Collision.pMyCollider->Get_Tag() == L"Col_Body")
	{
		_vector vOtherPosition = Collision.pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vOtherPosition);
		m_pTransformCom->Repersive(vDir, TimeDelta);
	}
}

void CPlayer::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
{
}

HRESULT CPlayer::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRendererCom, this)))
		return E_FAIL;
	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_AnimMesh", L"Com_Shader_AnimMesh",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;
	if (FAILED(Add_Component(LEVEL_STATIC, L"Model_Player", L"Com_Model",
		(CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	/* Status */

	CHealth::HEALTHDESC HealthDesc;
	HealthDesc.iMaxHP = 500;
	HealthDesc.iHP = 500;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Status_Health", L"Com_Health",
		(CComponent**)&m_pHealth, this, &HealthDesc)))
		return E_FAIL;

	CAttack::ATTACKDESC AttackDesc;
	AttackDesc.iDamage = 30;
	AttackDesc.iSkillDamage = 100;
	AttackDesc.isIgnoreDeffence = false;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Status_Attack", L"Com_Attack",
		(CComponent**)&m_pAttack, this, &AttackDesc)))
		return E_FAIL;

	CDeffence::DEFFENCEDESC DeffenceDesc;
	DeffenceDesc.iDeffence = 10;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Status_Deffence", L"Com_Deffence",
		(CComponent**)&m_pDeffence, this, &DeffenceDesc)))
		return E_FAIL;

	CInven::INVENDESC InvenDesc;
	InvenDesc.pAttack = m_pAttack;
	InvenDesc.pDeffence = m_pDeffence;
	InvenDesc.pHealth = m_pHealth;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Inven", L"Inven", 
		(CComponent**)&m_pInven, this, &InvenDesc)))
		return E_FAIL;

	/* Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	NaviDesc.iCurrentIndex = /*29*/0;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Navigation", L"Com_Navigation",
		(CComponent**)&m_pNavigationCom, this, &NaviDesc)))
		return E_FAIL;
	m_pTransformCom->Bind_Navigation(m_pNavigationCom);

	/* Collider */
	CBounding_AABB::AABBDESC AABBDesc;
	AABBDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	AABBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	AABBDesc.eGroup = CCollider::COL_PLAYER;
	AABBDesc.vOffset = _float3(0.f, 1.f, 0.f);
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_AABB", L"Col_Body", &AABBDesc)))
		return E_FAIL;

	CBounding_OBB::OBBDESC OBBDesc;
	OBBDesc.vExtents = _float3(1.5f, 0.5f, 1.f);
	OBBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	OBBDesc.eGroup = CCollider::COL_PLAYER_ATK;
	OBBDesc.vOffset = _float3(0.f, 1.f, 1.f);
	OBBDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_OBB", L"Col_Attack", &OBBDesc)))
		return E_FAIL;

	CBounding_Sphere::SPHEREDESC SphereDesc;
	SphereDesc.fRadius = 3.f;
	SphereDesc.eGroup = CCollider::COL_PLAYER_ATK;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);
	SphereDesc.vOffset = _float3(0.f, 0.f, 0.f);
	SphereDesc.isEnable = false;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_WheelWind", &SphereDesc)))
		return E_FAIL;

	/* Root */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Root", L"Com_Root",
		(CComponent**)&m_pRoot, this)))
		return E_FAIL;

	if (FAILED(m_pRoot->Add_Type(L"pShader", m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pRoot->Add_Type(L"pModel", m_pModelCom)))
		return E_FAIL;
	if (FAILED(m_pRoot->Add_Type(L"pRenderer", m_pRendererCom)))
		return E_FAIL;
	if (FAILED(m_pRoot->Add_Type(L"pTransform", m_pTransformCom)))
		return E_FAIL;
	if (FAILED(m_pRoot->Add_Type(L"pHealth", m_pHealth)))
		return E_FAIL;
	if (FAILED(m_pRoot->Add_Type(L"fTimeAcc", &m_fTimeAcc)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pActionCom = dynamic_cast<CPlayerAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, L"PlayerAction", this));
	if (nullptr == m_pActionCom)
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayer::Add_Parts()
{
	CWeapon::WEAPONDESC Desc;
	CSwordTrail::SWORDTRAILDESC TrailDesc;

	TrailDesc.iNumRect = 30.f;
	TrailDesc.vOffsetHigh = _float3(0.f, 0.f, 2.4f);
	TrailDesc.vOffsetLow = _float3(0.f, 0.f, 0.5f);
	TrailDesc.wstrTextureTag = L"Texture_SwordTrail";
	TrailDesc.iLevelIndex = LEVEL_GAMEPLAY;

	CSwordTrail* pSwordTrail = dynamic_cast<CSwordTrail*>(CGameInstance::GetInstance()->
		Clone_GameObject(LEVEL_GAMEPLAY, L"SwordTrail", L"PlayerSwordTrail", this, &TrailDesc));
	if (nullptr == pSwordTrail)
		return E_FAIL;

	const CBone* pBone = m_pModelCom->Get_Bone("Bone_War_Weapon_Sword");
	if (nullptr == pBone)
		return E_FAIL;

	XMStoreFloat4x4(&Desc.OffsetMatrix, pBone->Get_OffsetMatrix());
	XMStoreFloat4x4(&Desc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	Desc.pCombinedTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	Desc.pNotMoveCombinedTransformationMatrix = pBone->Get_NotMoveCombinedTransformationMatrixPtr();
	Desc.pWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	Desc.SpeedPerSec = 7.0;
	Desc.RotationPerSec = XMConvertToRadians(90.0);

	Desc.wstrModelTag = L"Model_PlayerWeapon";

	Desc.pSwordTrail = pSwordTrail;

	if (FAILED(CGameObject::Add_Parts(LEVEL_STATIC, L"Weapon", L"Weapon", this, &Desc)))
		return E_FAIL;

	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vEye = _float4(1.f, 1.f, 1.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

	CameraDesc.fFov = XMConvertToRadians(60.f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.3f;
	CameraDesc.fFar = 150.f;

	CameraDesc.SpeedPerSec = 10.f;
	CameraDesc.RotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(CGameObject::Add_Parts(LEVEL_STATIC, L"Camera_Free", L"Camera_Free", this, &CameraDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CPlayer::Set_Shader_Resources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 InputMatrix = m_pTransformCom->Get_WorldFloat4x4();
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &InputMatrix)))
		return E_FAIL;

	InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &InputMatrix)))
		return E_FAIL;

	InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &InputMatrix)))
		return E_FAIL;

	if (FAILED(m_pActionCom->Bind_HitTimeAcc(m_pShaderCom, "g_fTimeAcc")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayer::Set_Shader_Shadow_Resources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 InputFloat4x4 = m_pTransformCom->Get_WorldFloat4x4();
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &InputFloat4x4)))
		return E_FAIL;

	InputFloat4x4 = pGameInstance->Get_LightViewFloat4x4(0);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_LightViewMatrix", &InputFloat4x4)))
		return E_FAIL;

	InputFloat4x4 = pGameInstance->Get_LightProjFloat4x4(m_pContext);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_LightProjMatrix", &InputFloat4x4)))
		return E_FAIL;

	if (FAILED(m_pActionCom->Bind_HitTimeAcc(m_pShaderCom, "g_fTimeAcc")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CPlayer::Set_ZommBlur(const _double& TimeDelta)
{
	static _bool isFirst = { true };
	static _float fTimeAcc = { 0.f };

	fTimeAcc += TimeDelta * 25.f;

	if (true == isFirst)
	{
		m_pRendererCom->Set_Pass(CRenderer::PASS_ZOOMBLUR);
		isFirst = false;
	}

	m_pRendererCom->Set_ZoomInBlurData(5.f - fTimeAcc, 7.f);

	_float fAlpha = { 0.f };
	if (2.5 > fTimeAcc)
	{
		fAlpha += TimeDelta * 10.f;
	}
	else
	{
		fAlpha -= TimeDelta * 10.f;
	}

	m_pRendererCom->Set_Focus_Alpha(fAlpha);

	if (5.f < fTimeAcc)
	{
		fTimeAcc = 0.f;
		isFirst = true;
		m_isRenderZoom = false;
		m_pRendererCom->Set_Pass(CRenderer::PASS_POSTPROCESSING);
	}
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CPlayer* CPlayer::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRoot);

	Safe_Release(m_pActionCom);

	Safe_Release(m_pHealth);
	Safe_Release(m_pAttack);
	Safe_Release(m_pDeffence);

	Safe_Release(m_pInven);

	CGameObject3D::Free();
}
