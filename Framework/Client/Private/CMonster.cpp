#include "stdafx.h"
#include "CMonster.h"

#include "CGameInstance.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject3D(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	:CGameObject3D(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_pTransformCom->Set_Matrix(reinterpret_cast<MONSTERDESC*>(pArg)->WorldMatrix);
		m_pTransformCom->Set_Angle(reinterpret_cast<MONSTERDESC*>(pArg)->vAngle);
	}

	return S_OK;
}

void CMonster::Tick(const _double& TimeDelta)
{
	m_pTransformCom->Animation_Movement(m_pModelCom, TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Down(DIK_2))
		m_pModelCom->Change_Animation("Idle");

	if (pGameInstance->Key_Down(DIK_3))
		m_pModelCom->Change_Animation("Attack");

	if (pGameInstance->Key_Down(DIK_4))
		m_pModelCom->Change_Animation("Knockback");

	if (pGameInstance->Key_Down(DIK_5))
		m_pModelCom->Change_Animation("Get");

	Safe_Release(pGameInstance);

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CMonster::AfterFrustumTick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		pGameInstance->Add_Collider(COL_ENEMY, m_pColliderCom);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}

	Safe_Release(pGameInstance);
}

void CMonster::Late_Tick(const _double& TimeDelta)
{
	/* 여기는 콜라이더가 객체의 상태를 변경 시켰고(On_Collision) 그에 대한 처리를 하는 부분 */

	m_pColliderCom->On_Collision(this, TimeDelta);
}

HRESULT CMonster::Render()
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TYPE_DIFFUSE);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();

#endif

	return S_OK;
}

void CMonster::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (Collision.pMyCollider->Get_Tag() == L"Col_Body")
		m_pTransformCom->Repersive(XMLoadFloat3(&Collision.vOtherMoveDirection), TimeDelta);
}

void CMonster::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (Collision.pMyCollider->Get_Tag() == L"Col_Body")
		m_pTransformCom->Repersive(XMLoadFloat3(&Collision.vOtherMoveDirection), TimeDelta);
}

void CMonster::OnCollisionExit(const _double& TimeDelta)
{
}

HRESULT CMonster::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_AnimMesh",
		L"Com_Shader", (CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer",
		L"Com_Renderer", (CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Model_Goblin",
		L"Com_Model", (CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	CNavigation::NAVIGATIONDESC NaviDesc;
	NaviDesc.iCurrentIndex = 0;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Navigation", L"Com_Navigation",
		(CComponent**)&m_pNavigationCom, this, &NaviDesc)))
		return E_FAIL;
	m_pTransformCom->Bind_Navigation(m_pNavigationCom);

	CBounding_Sphere::SPHEREDESC SphereDesc;
	SphereDesc.fRadius = 1.f;
	SphereDesc.vPosition = _float3(0.f, 1.f, 0.f);

	if (FAILED(Add_Component(LEVEL_STATIC, L"Collider_Sphere", L"Col_Body",
		(CComponent**)&m_pColliderCom, this, &SphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Set_Shader_Resources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix",
		&m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix",
		&pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix",
		&pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ))))
		return E_FAIL;

	CLight::LIGHTDESC LightDesc = *pGameInstance->Get_LightDesc(0);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_LightPosition",
		&LightDesc.vPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_LightDirection",
		&LightDesc.vDirection, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_LightDiffuse",
		&LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_LightSpecular",
		&LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_LightAmbient",
		&LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CameraPosition",
		&pGameInstance->Get_Camera_Position(), sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster* pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CMonster* CMonster::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CMonster* pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonster::Free()
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	CGameObject3D::Free();
}
