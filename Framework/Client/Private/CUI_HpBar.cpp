#include "CUI_HpBar.h"

#include "CGameInstance.h"

CUI_HpBar::CUI_HpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObjectUI(pDevice, pContext)
{
}

CUI_HpBar::CUI_HpBar(const CUI_HpBar& rhs)
	: CGameObjectUI(rhs)
{
}

HRESULT CUI_HpBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_HpBar::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UIHPBARDESC Desc = *reinterpret_cast<UIHPBARDESC*>(pArg);

	m_pParentMatrix = Desc.pParentMatrix;
	m_iPassNum = Desc.iPassNum;
	m_vOffset = Desc.vOffset;
	m_pMaxHp = Desc.pMaxHp;
	m_pHp = Desc.pHp;

	if (FAILED(CGameObjectUI::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(Desc.iTextureLevelIndex, Desc.wstrTextureTag.c_str(),
		L"Com_Texture", (CComponent**)&m_pTextureCom, this)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_HpBar::Tick(const _double& TimeDelta)
{
	/* 오버레이는 객체를 따라가야됨 */
	_float3 vPos = _float3(m_pParentMatrix->_41, m_pParentMatrix->_42, m_pParentMatrix->_43);
	_vector vPosition = XMLoadFloat3(&vPos);
	_vector vOffset = XMLoadFloat3(&m_vOffset);

	vPosition += vOffset;

	vPosition = XMVector3TransformCoord(vPosition, CPipeLine::GetInstance()->Get_Transform_Matrix(CPipeLine::STATE_VIEW));
	vPosition = XMVector3TransformCoord(vPosition, CPipeLine::GetInstance()->Get_Transform_Matrix(CPipeLine::STATE_PROJ));

	D3D11_VIEWPORT ViewPortDesc;
	_uint iNumView = { 1 };

	m_pContext->RSGetViewports(&iNumView, &ViewPortDesc);

	CUI_HpBar::UIDESC UIDesc;
	UIDesc.m_fX = ((XMVectorGetX(vPosition) + 1.0f) * 0.5f) * ViewPortDesc.Width;
	UIDesc.m_fY = ((-XMVectorGetY(vPosition) + 1.0f) * 0.5f) * ViewPortDesc.Height;
	UIDesc.m_fDepth = 0.f;

	Set_Position(_float3(UIDesc.m_fX, UIDesc.m_fY, UIDesc.m_fDepth));
}

void CUI_HpBar::Late_Tick(const _double& TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_HpBar::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iPassNum);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CUI_HpBar::Add_Components()
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

HRESULT CUI_HpBar::SetUp_ShaderResources()
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

	_float fData = _float(*m_pMaxHp);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaxHp", &fData, sizeof(_float))))
		return E_FAIL;

	fData = _float(*m_pHp);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHp", &fData, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CUI_HpBar* CUI_HpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_HpBar* pInstance = new CUI_HpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_HpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_HpBar* CUI_HpBar::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CUI_HpBar* pInstance = new CUI_HpBar(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_HpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HpBar::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	CGameObjectUI::Free();
}
