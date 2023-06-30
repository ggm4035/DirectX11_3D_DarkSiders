#include "stdafx.h"
#include "CUI_Rect.h"

#include "CGameInstance.h"

CUI_Rect::CUI_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObjectUI(pDevice, pContext)
{
}

CUI_Rect::CUI_Rect(const CUI_Rect& rhs)
	: CGameObjectUI(rhs)
{
}

HRESULT CUI_Rect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Rect::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CGameObjectUI::UIDESC UIDesc;
	UIDesc.m_fX = _float(g_iWinSizeX >> 1);
	UIDesc.m_fY = _float(g_iWinSizeY >> 1);
	UIDesc.m_fSizeX = (_float)g_iWinSizeX;
	UIDesc.m_fSizeY = (_float)g_iWinSizeY;

	if (FAILED(CGameObjectUI::Initialize(iLevelIndex, pOwner, &UIDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Rect::Tick(const _double& TimeDelta)
{
}

void CUI_Rect::Late_Tick(const _double& TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CUI_Rect::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Rect::Add_Components()
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

	// 텍스처만 바뀌면 나머지 사용가능할지도?
	if (FAILED(Add_Component(LEVEL_LOGO, L"Texture_Logo",
		L"Com_Texture_Logo", (CComponent**)&m_pTextureCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Rect::SetUp_ShaderResources()
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

CUI_Rect* CUI_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Rect* pInstance = new CUI_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObjectUI* CUI_Rect::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CUI_Rect* pInstance = new CUI_Rect(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Rect::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	CGameObjectUI::Free();
}
