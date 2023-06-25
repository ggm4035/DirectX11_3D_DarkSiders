#include "stdafx.h"
#include "CPlayer.h"

#include "CGameInstance.h"

#include "CShader.h"
#include "CTexture.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject3D(pDevice, pContext)
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

HRESULT CPlayer::Initialize(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLayerIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//if (nullptr != pArg)
		//m_pTransformCom->Set_Matrix(reinterpret_cast<PLAYERDESC*>(pArg)->WorldMatrix);

	return S_OK;
}

void CPlayer::Tick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Pressing(DIK_UP))
	{
		m_pTransformCom->Go_Straight(TimeDelta);
		m_pModelCom->Set_AnimIndex(1);
	}

	if (pGameInstance->Key_Up(DIK_UP))
		m_pModelCom->Set_AnimIndex(0);

	if (pGameInstance->Key_Pressing(DIK_LEFT))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_RIGHT))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);

	if (pGameInstance->Key_Down(DIK_1))
		m_pModelCom->Set_AnimIndex(3);

	if (pGameInstance->Key_Down(DIK_2))
		m_pModelCom->Set_AnimIndex(4);

	if (pGameInstance->Key_Down(DIK_3))
		m_pModelCom->Set_AnimIndex(5);

	Safe_Release(pGameInstance);

	m_pModelCom->Play_Animation(TimeDelta, m_pTransformCom);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

void CPlayer::Late_Tick(const _double& TimeDelta)
{

}

HRESULT CPlayer::Render()
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

	//m_pNavigationCom->Render_Navigation();

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();

#endif

	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_AnimMesh", L"Com_Shader_AnimMesh",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Model_Player", L"Com_Model_Player",
		(CComponent**)&m_pModelCom, this)))
		return E_FAIL;

	CNavigation::NAVIGATIONDESC NaviDesc;
	NaviDesc.iCurrentIndex = 0;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Navigation", L"Com_Navigation",
		(CComponent**)&m_pNavigationCom, this, &NaviDesc)))
		return E_FAIL;
	m_pTransformCom->Bind_Navigation(m_pNavigationCom);

	/* Collider */
	CBounding_AABB::AABBDESC AABBDesc;
	AABBDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	AABBDesc.vPosition = _float3(0.f, 1.f, 0.f);

	if (FAILED(Add_Component(LEVEL_STATIC, L"Collider_AABB", L"Com_Collider_AABB",
		(CComponent**)&m_pColliderCom, this, &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Set_Shader_Resources()
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

CPlayer* CPlayer::Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(iLayerIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	CGameObject3D::Free();
}
