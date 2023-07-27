
#include "CPlayer.h"

#include "CGameInstance.h"

#include "CPlayerAction.h"
#include "CWeapon.h"

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

	CGameInstance::GetInstance()->Set_Player(this);

	return S_OK;
}

void CPlayer::Tick(const _double& TimeDelta)
{
	CGameObject3D::Tick(TimeDelta);
	
	m_pActionCom->Tick(TimeDelta);

	m_pTransformCom->Animation_Movement(m_pModelCom, TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta, m_pNavigationCom);

	Tick_Colliders(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::AfterFrustumTick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 콜라이더 매니저에 콜라이더 추가 */
	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0))
	{
		if (FAILED(Add_Colliders_To_Manager()))
		{
			MSG_BOX("Failed to Add Colliders To Manager");
			Safe_Release(pGameInstance);
			return;
		}

		for (auto Pair : m_Parts)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, static_cast<CGameObject*>(Pair.second));

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

		if (true == m_isRender && FAILED(Add_Colliders_Debug_Render_Group(m_pRendererCom)))
			return;
#ifdef _DEBUG
#endif
	}

	Safe_Release(pGameInstance);
}

/* 이후 게임 인스턴스에서 콜라이더들의 충돌을 진행 */

void CPlayer::Late_Tick(const _double& TimeDelta)
{
	CGameObject3D::Late_Tick(TimeDelta);

	/* 충돌 이후 작업을 진행 (메시지 받은거 처리하는 구간임) */

	On_Colisions(TimeDelta);
}

/* 모든 게임오브젝트의 latetick이 끝나면 게임 인스턴스에서 콜라이더의 모든 리스트를 비움 */

HRESULT CPlayer::Render()
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TYPE_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TYPE_NORMALS);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CPlayer::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (Collision.pMyCollider->Get_Tag() == L"Col_Attack" &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		//cout << "데미지" << endl;
		Collision.pOther->Get_Damaged();
	}
}

void CPlayer::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (Collision.pMyCollider->Get_Tag() == L"Col_Body")
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

	/* Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	NaviDesc.iCurrentIndex = 290;
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

	/* Actions */
	if (FAILED(Add_Component(LEVEL_STATIC, L"PlayerAction", L"Com_Action",
		(CComponent**)&m_pActionCom, this)))
		return E_FAIL;

	/* Setup Notify */
	if (FAILED(m_pModelCom->Setup_Notifys()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_Parts()
{
	CWeapon::WEAPONDESC Desc;

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

	if (FAILED(CGameObject::Add_Parts(LEVEL_STATIC, L"Weapon", L"Weapon_Player", this, &Desc)))
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

	Safe_Release(pGameInstance);

	return S_OK;
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
	Safe_Release(m_pActionCom);
	Safe_Release(m_pModelCom);

	CGameObject3D::Free();
}
