#include "stdafx.h"
#include "CBackGround.h"

#include "CGameInstance.h"

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObjectUI(pDevice, pContext)
{
}

CBackGround::CBackGround(const CBackGround& rhs)
	: CGameObjectUI(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround::Initialize(CComponent* pOwner, void* pArg)
{
	CGameObjectUI::UIDESC UIDesc;
	UIDesc.m_fX = _float(g_iWinSizeX >> 1);
	UIDesc.m_fY = _float(g_iWinSizeY >> 1);
	UIDesc.m_fSizeX = (_float)g_iWinSizeX;
	UIDesc.m_fSizeY = (_float)g_iWinSizeY;

	if (FAILED(CGameObjectUI::Initialize(pOwner, &UIDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Tick(const _double& TimeDelta)
{
}

void CBackGround::Late_Tick(const _double& TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CBackGround::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CBackGround::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Renderer",
		L"Com_Renderer", (CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect",
		L"Com_Buffer", (CComponent**)&m_pBufferCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxTex",
		L"Com_Shader_VtxTex", (CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_LOGO, L"Prototype_Component_Texture_Logo",
		L"Com_Texture_Test", (CComponent**)&m_pTextureCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &pGameInstance->Get_UI_View_Float4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &pGameInstance->Get_UI_Proj_Float4x4(g_iWinSizeX, g_iWinSizeY))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackGround* pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObjectUI* CBackGround::Clone(CComponent* pOwner, void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackGround::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	CGameObjectUI::Free();
}
