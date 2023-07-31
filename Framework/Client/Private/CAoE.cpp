#include "CAoE.h"

#include "CGameInstance.h"

CAoE::CAoE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CAoE::CAoE(const CAoE& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CAoE::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		AOEDESC Desc = *reinterpret_cast<AOEDESC*>(pArg);
		m_iPassNum = Desc.iPassNum;
		m_strShaderResourceTag = Desc.strShaderResourceTag;

		if (FAILED(Add_Component(Desc.iTextureLevel, Desc.wstrTextureTag.c_str(), L"Com_Texture",
			(CComponent**)&m_pTextureCom, this)))
			return E_FAIL;
	}

	return S_OK;
}

void CAoE::Tick(const _float4x4& WorldMatrix)
{
	m_pTransformCom->Set_Matrix(WorldMatrix);
}

void CAoE::AfterFrustumTick(const _double& TimeDelta)
{
}

void CAoE::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CAoE::Render()
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iPassNum)))
		return E_FAIL;

	if (FAILED(m_pBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAoE::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"VIBuffer_Rect", L"Com_Buffer",
		(CComponent**)&m_pBufferCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_VtxTex", L"Com_Shader",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAoE::Set_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4Ptr())))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &InputMatrix)))
		return E_FAIL;

	InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &InputMatrix)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, m_strShaderResourceTag)))
		return E_FAIL;

	return S_OK;
}

CAoE* CAoE::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAoE* pInstance = new CAoE(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAoE");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAoE* CAoE::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CAoE* pInstance = new CAoE(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAoE");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAoE::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);

	CGameObject3D::Free();
}
