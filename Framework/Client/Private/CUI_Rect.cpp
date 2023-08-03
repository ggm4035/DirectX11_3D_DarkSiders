
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
	if (nullptr == pArg)
		return E_FAIL;

	UIRECTDESC Desc = *reinterpret_cast<UIRECTDESC*>(pArg);

	m_iPassNum = Desc.iPassNum;
	m_pMaxHp = Desc.pMaxHp;
	m_pHp = Desc.pHp;
	m_pCoolTime = Desc.pCoolTime;

	if (FAILED(CGameObjectUI::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(Desc.iTextureLevelIndex, Desc.wstrTextureTag.c_str(),
		L"Com_Texture", (CComponent**)&m_pTextureCom, this)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Rect::Tick(const _double& TimeDelta)
{
	m_fTimeAcc += TimeDelta;
}

void CUI_Rect::Late_Tick(const _double& TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Rect::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iPassNum);

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

	return S_OK;
}

HRESULT CUI_Rect::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4 InputMatrix = pGameInstance->Get_UI_View_Float4x4();
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &InputMatrix)))
		return E_FAIL;

	InputMatrix = pGameInstance->Get_UI_Proj_Float4x4(g_iWinSizeX, g_iWinSizeY);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &InputMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (nullptr != m_pCoolTime)
	{
		_float fData = *m_pCoolTime / 5.f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoolTime", &fData, sizeof(_float))))
			return E_FAIL;
	}

	if (nullptr != m_pMaxHp)
	{
		_float fPer = (_float)*m_pHp / *m_pMaxHp;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fHpPer", &fPer, sizeof(_float))))
			return E_FAIL;
	}

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
