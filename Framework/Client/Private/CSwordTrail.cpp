#include "CSwordTrail.h"

#include "CGameInstance.h"

CSwordTrail::CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CSwordTrail::CSwordTrail(const CSwordTrail& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CSwordTrail::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	SWORDTRAILDESC Desc = *reinterpret_cast<SWORDTRAILDESC*>(pArg);

	m_vOffsetLow = Desc.vOffsetLow;
	m_vOffsetHigh = Desc.vOffsetHigh;

	CVIBuffer_Trail::TRAILDESC TrailDesc;
	TrailDesc.iNumRect = Desc.iNumRect;
	TrailDesc.pHightPosition = &m_vHighPosition;
	TrailDesc.pLowPosition = &m_vLowPosition;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"VIBuffer_Trail", L"Com_Buffer",
		(CComponent**)&m_pBufferCom, this, &TrailDesc)))
		return E_FAIL;

	if (0 < Desc.wstrTextureTag.size())
	{
		if (FAILED(Add_Component(Desc.iLevelIndex, Desc.wstrTextureTag, L"Com_Texture",
			(CComponent**)&m_pTextureCom, this)))
			return E_FAIL;
	}

	return S_OK;
}

void CSwordTrail::Tick(const _float4x4& WorldMatrix, const _double& TimeDelta)
{
	XMStoreFloat3(&m_vLowPosition, (XMMatrixTranslation(m_vOffsetLow.x, m_vOffsetLow.y, m_vOffsetLow.z) * XMLoadFloat4x4(&WorldMatrix)).r[3]);
	XMStoreFloat3(&m_vHighPosition, (XMMatrixTranslation(m_vOffsetHigh.x, m_vOffsetHigh.y, m_vOffsetHigh.z) * XMLoadFloat4x4(&WorldMatrix)).r[3]);

	m_pBufferCom->Tick(WorldMatrix, TimeDelta);
}

void CSwordTrail::AfterFrustumTick(const _double& TimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

void CSwordTrail::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CSwordTrail::Render()
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(7);

	m_pBufferCom->Render();
	
	return S_OK;
}

HRESULT CSwordTrail::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRenderer, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_VtxTex", L"Com_Shader_VtxTex",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwordTrail::Set_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 matrix;
	XMStoreFloat4x4(&matrix, XMMatrixIdentity());
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &matrix)))
		return E_FAIL;

	_float4x4 InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &InputMatrix)))
		return E_FAIL;

	InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &InputMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

CSwordTrail* CSwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwordTrail* pInstance = new CSwordTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSwordTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CSwordTrail* CSwordTrail::Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg)
{
	CSwordTrail* pInstance = new CSwordTrail(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CSwordTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSwordTrail::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRenderer);

	CGameObject3D::Free();
}
