
#include "CMonster.h"

#include "CRoot.h"
#include "CGameInstance.h"
#include "CUI_HpBar.h"
#include "CPlayer.h"
#include "CAoE.h"
#include "CSoul.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject3D(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	:CGameObject3D(rhs)
{
}

void CMonster::Get_Damaged(const CAttack* pAttack)
{
	_int iDeffence = m_pDeffence->Get_Deffence();
	_int iDamage = pAttack->Get_Damage();

	if (false == pAttack->isIgnoreDeffence())
		Saturate(iDamage -= iDeffence, 0, iDamage);

	m_pHealth->Damaged(iDamage);
}

void CMonster::Get_Skill_Damaged(const CAttack* pAttack)
{
	_int iDeffence = m_pDeffence->Get_Deffence();
	_int iDamage = pAttack->Get_Skill_Damage();

	m_pHealth->Damaged(iDamage);
}

HRESULT CMonster::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(CMonster::Add_Components()))
		return E_FAIL;

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition = XMVector3TransformCoord(vPosition, CPipeLine::GetInstance()->Get_Transform_Matrix(CPipeLine::STATE_VIEW));
	vPosition = XMVector3TransformCoord(vPosition, CPipeLine::GetInstance()->Get_Transform_Matrix(CPipeLine::STATE_PROJ));

	D3D11_VIEWPORT ViewPortDesc;
	_uint iNumView = { 1 };

	m_pContext->RSGetViewports(&iNumView, &ViewPortDesc);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_HpBar::UIHPBARDESC UIDesc;
	UIDesc.m_fX = ((XMVectorGetX(vPosition) + 1.0f) * 0.5f) * ViewPortDesc.Width;
	UIDesc.m_fY = ((-XMVectorGetY(vPosition) + 1.0f) * 0.5f) * ViewPortDesc.Height;
	UIDesc.m_fDepth = XMVectorGetZ(vPosition);
	UIDesc.m_fSizeX = 50.f;
	UIDesc.m_fSizeY = 10.f;
	UIDesc.wstrTextureTag = L"Texture_UI_UnitFrame_HpBar";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	UIDesc.vOffset = _float3(0.f, 2.f, 0.f);
	UIDesc.pHealth = m_pHealth;
	UIDesc.iPassNum = 3;

	CGameObject* pGameObject = pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"UI_HpBar", L"UI_HealthBar", this, &UIDesc);
	m_pHealthBar = dynamic_cast<CUI_HpBar*>(pGameObject);
	if (nullptr == m_pHealthBar)
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (nullptr != pArg)
	{
		MONSTERDESC Desc = *static_cast<MONSTERDESC*>(pArg);
		m_pTransformCom->Set_Matrix(Desc.WorldMatrix);
		m_pTransformCom->Set_Angle(Desc.vAngle);

		CNavigation::NAVIGATIONDESC NaviDesc;
		NaviDesc.iCurrentIndex = Desc.iNavigationIndex;
		if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Navigation", L"Com_Navigation",
			(CComponent**)&m_pNavigationCom, this, &NaviDesc)))
			return E_FAIL;
		m_pTransformCom->Bind_Navigation(m_pNavigationCom);
		m_isSpawn = Desc.isSpawn;
		if (true == m_isSpawn)
		{
			_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPosition.m128_f32[1] = vPosition.m128_f32[1] - 2.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}

	return S_OK;
}

void CMonster::Tick(const _double& TimeDelta)
{
	m_fTimeAcc += TimeDelta;

	CGameObject3D::Tick(TimeDelta);

	m_pRoot->Tick(TimeDelta); /* 객체가 움직여 애니메이션 변경 */

	m_pModelCom->Play_Animation(TimeDelta, m_pNavigationCom);

	m_pTransformCom->Animation_Movement(m_pModelCom, TimeDelta); /* 객체가 진짜 움직임 */

	m_pHealthBar->Tick(TimeDelta);

	Tick_Colliders(m_pTransformCom->Get_WorldMatrix());
}

void CMonster::AfterFrustumTick(const _double& TimeDelta)
{
	if (false == m_isRender)
		return;

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

		if (1.f > m_pHealth->Get_Current_HP_Percent() && nullptr != m_pHealthBar)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, m_pHealthBar);

#ifdef _DEBUG
		/*if (true == m_isRender && FAILED(Add_Colliders_Debug_Render_Group(m_pRendererCom)))
			return;*/
#endif
	}

	Safe_Release(pGameInstance);
}

void CMonster::Late_Tick(const _double& TimeDelta)
{
	On_Colisions(TimeDelta);
}
/* PassNum
0 : Default
1 : Hit
*/
HRESULT CMonster::Render(/*const _uint& iPassIndex*/)
{
	if (FAILED(CMonster::Set_Shader_Resources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint iMeshIndex = 0; iMeshIndex < iNumMeshes; ++iMeshIndex)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", iMeshIndex);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, TYPE_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", iMeshIndex, TYPE_NORMALS);

		if (FAILED(m_pShaderCom->Begin(m_iPassNum)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(iMeshIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CMonster::Dead_Motion(const _double& TimeDelta)
{
	if (true == m_isDeadMotionFirst)
	{
		for (auto& pSoul : m_vecSouls)
		{
			pSoul->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}
		m_fTimeAcc = 0.f;
		m_isDeadMotionFirst = false;
	}
	m_fHitTimeAcc += TimeDelta;
}

void CMonster::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
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
}

void CMonster::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if ((Collision.pOtherCollider->Get_Collider_Group() == CCollider::COL_STATIC ||
		Collision.pOtherCollider->Get_Tag() == L"Col_Body") &&
		Collision.pMyCollider->Get_Tag() == L"Col_Body")
	{
		_vector vOtherPosition = Collision.pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vOtherPosition);
		m_pTransformCom->Repersive(vDir, TimeDelta);
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isRangeInPlayer = true;
	}

	if (Collision.pMyCollider->Get_Tag() == L"Col_Melee_Range" &&
		nullptr != dynamic_cast<CPlayer*>(Collision.pOther))
	{
		m_isAbleAttack = true;
	}
}

void CMonster::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
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

HRESULT CMonster::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_AnimMesh",
		L"Com_Shader", (CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer",
		L"Com_Renderer", (CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_Dissolve",
		L"Com_DissolveTex", (CComponent**)&m_pDissolveTexture, this)))
		return E_FAIL;

	/* Status */
	CHealth::HEALTHDESC HealthDesc;
	HealthDesc.iMaxHP = 1;
	HealthDesc.iHP = 1;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Status_Health", L"Com_Health",
		(CComponent**)&m_pHealth, this, &HealthDesc)))
		return E_FAIL;

	CAttack::ATTACKDESC AttackDesc;
	AttackDesc.iDamage = 1;
	AttackDesc.isIgnoreDeffence = false;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Status_Attack", L"Com_Attack",
		(CComponent**)&m_pAttack, this, &AttackDesc)))
		return E_FAIL;

	CDeffence::DEFFENCEDESC DeffenceDesc;
	DeffenceDesc.iDeffence = 0;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Status_Deffence", L"Com_Deffence",
		(CComponent**)&m_pDeffence, this, &DeffenceDesc)))
		return E_FAIL;

	/* Root */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Root", L"Com_Root",
		(CComponent**)&m_pRoot, this)))
		return E_FAIL;

	/* BlackBoard */
	m_pRoot->Add_Type(L"vDirection", _float3());
	m_pRoot->Add_Type(L"fTimeAcc", &m_fTimeAcc);

	m_pRoot->Add_Type(L"fHitTimeAcc", &m_fHitTimeAcc);

	m_pRoot->Add_Type(L"isSpawn", &m_isSpawn);
	m_pRoot->Add_Type(L"isRemove", &m_isRemove);
	m_pRoot->Add_Type(L"isRender", &m_isRender);
	m_pRoot->Add_Type(L"isAbleAttack", &m_isAbleAttack);
	m_pRoot->Add_Type(L"isRangeInPlayer", &m_isRangeInPlayer);

	m_pRoot->Add_Type(L"pTarget", CGameInstance::GetInstance()->Get_Player());
	m_pRoot->Add_Type(L"pHealth", m_pHealth);

	return S_OK;
}

HRESULT CMonster::Set_Shader_Resources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fHitTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pDissolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CMonster::Free()
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRoot);

	Safe_Release(m_pHealthBar);

	for (auto& pSoul : m_vecSouls)
		Safe_Release(pSoul);

	Safe_Release(m_pHealth);
	Safe_Release(m_pAttack);
	Safe_Release(m_pDeffence);

	Safe_Release(m_pDissolveTexture);

	CGameObject3D::Free();
}
