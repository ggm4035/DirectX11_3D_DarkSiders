#include "stdafx.h"
#include "CWeapon.h"

#include "CGameInstance.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CParts(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	:CParts(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CParts::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		WEAPONDESC Desc = *reinterpret_cast<WEAPONDESC*>(pArg);

		if (FAILED(Add_Component(LEVEL_STATIC, Desc.wstrModelTag.c_str(), L"Com_Model_Weapon",
			(CComponent**)&m_pModelCom, this)))
			return E_FAIL;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CWeapon::Tick(const _double& TimeDelta)
{
	_matrix BoneMatrix;
	CModel* pModel = dynamic_cast<CModel*>(dynamic_cast<CGameObject3D*>(m_pOwner)->Get_Component(L"Com_Model"));

	if(false == pModel->isLoopAnimation())
		BoneMatrix = XMLoadFloat4x4(&m_OffsetMatrix) * XMLoadFloat4x4(m_pNotMoveCombinedTransformationMatrix) * XMLoadFloat4x4(&m_PivotMatrix);
	else
		BoneMatrix = XMLoadFloat4x4(&m_OffsetMatrix) * XMLoadFloat4x4(m_pCombinedTransformationMatrix) * XMLoadFloat4x4(&m_PivotMatrix);

	for (_uint i = 0; i < 3; ++i)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));
}

HRESULT CWeapon::Render()
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TEXTURETYPE::TYPE_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CWeapon::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_Mesh", L"Com_Shader_Mesh",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Set_Shader_Resources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix",
		&m_WorldMatrix)))
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

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon* pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWeapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CWeapon* CWeapon::Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CWeapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWeapon::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);

	CParts::Free();
}
