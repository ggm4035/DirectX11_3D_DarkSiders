#include "CSpawn.h"

#include "CGameInstance.h"

CSpawn::CSpawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CSpawn::CSpawn(const CSpawn& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CSpawn::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CSpawn::Tick(const _float4x4& WorldMatrix, const _double& TimeDelta)
{
	m_fTimeAcc += TimeDelta * 3.f;
	m_fScale = limitFunc(m_fTimeAcc, 2.f, 2.f, 0.f);

	if (true == m_isFirst)
	{
		m_pTransformCom->Set_Matrix(WorldMatrix);
		m_vPosition = _float4(WorldMatrix._41, WorldMatrix._42 + 2.2f, WorldMatrix._43, 1.f);
		m_isFirst = false;
	}
	_vector vPosition = XMLoadFloat4(&m_vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Set_Scales(_float3(m_fScale, m_fScale, m_fScale));
	m_pTransformCom->Turn_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);
}

void CSpawn::AfterFrustumTick(const _double& TimeDelta)
{
}

void CSpawn::Late_Tick(const _double& TimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CSpawn::Render()
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(12)))
		return E_FAIL;

	if (FAILED(m_pBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpawn::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_Spawn", L"Com_Texture",
		(CComponent**)&m_pTextureCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"VIBuffer_Rect", L"Com_Buffer",
		(CComponent**)&m_pBufferCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_VtxTex", L"Com_Shader",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpawn::Set_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlphaTexture")))
		return E_FAIL;

	return S_OK;
}

CSpawn* CSpawn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpawn* pInstance = new CSpawn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSpawn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CSpawn* CSpawn::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CSpawn* pInstance = new CSpawn(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CSpawn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSpawn::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);

	CGameObject3D::Free();
}
