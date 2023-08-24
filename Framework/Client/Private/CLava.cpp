#include "CLava.h"

#include "CGameInstance.h"

CLava::CLava(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CLava::CLava(const CLava& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CLava::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLava::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_float4x4 WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixScaling(400.f, 400.f, 400.f) *
		XMMatrixRotationX(XMConvertToRadians(90.f)) *
		XMMatrixTranslation(230.f, 1.f, 250.f));

	m_pTransformCom->Set_Matrix(WorldMatrix);

	return S_OK;
}

void CLava::Tick(const _double& TimeDelta)
{
	m_ftimeAcc += TimeDelta * 0.0025f;
}

void CLava::Late_Tick(const _double& TimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
}

HRESULT CLava::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CLava::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer",
		L"Com_Renderer", (CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"VIBuffer_Rect",
		L"Com_Buffer", (CComponent**)&m_pBufferCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_VtxTex",
		L"Com_Shader_VtxTex", (CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_Lava",
		L"Com_Texture_Lava", (CComponent**)&m_pTextureCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLava::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	_float fDetail = 10.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDetail", &fDetail, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_ftimeAcc, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLava* CLava::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLava* pInstance = new CLava(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLava");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CLava* CLava::Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg)
{
	CLava* pInstance = new CLava(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CLava");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLava::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

	CGameObject3D::Free();
}
