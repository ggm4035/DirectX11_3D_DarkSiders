#include "CArmorCore.h"

#include "CGameInstance.h"
#include "CCurrency.h"
#include "CUI_Rect.h"
#include "CInven.h"
#include "CDeffence.h"

CArmorCore::CArmorCore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObjectUI(pDevice, pContext)
{
}

CArmorCore::CArmorCore(const CArmorCore& rhs)
	: CGameObjectUI(rhs)
{
}

HRESULT CArmorCore::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObjectUI::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	UIDESC Desc = *static_cast<UIDESC*>(pArg);

	CUI_Rect::UIRECTDESC UIDesc;
	UIDesc.m_fX = 0.f;
	UIDesc.m_fY = 0.f;
	UIDesc.m_fSizeX = Desc.m_fSizeX;
	UIDesc.m_fSizeY = Desc.m_fSizeY;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_Focus";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 11;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UI_Focus",
		(CComponent**)&m_pFocus, nullptr, &UIDesc)))
		return E_FAIL;

	m_pCurrentTexture = m_pTextures[0];

	return S_OK;
}

void CArmorCore::Tick(const _float3& _vPosition)
{
	Set_Position(_vPosition);
	_float3 vPosition = m_vPosition;

	vPosition.x += 1.f;
	vPosition.z = 0.03f;
	m_pFocus->Set_Position(vPosition);
}

void CArmorCore::Late_Tick(CInven* pInven, const _double& TimeDelta)
{
	_matrix WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_float fSizeX = XMVectorGetX(XMVector3Length(WorldMatrix.r[0]));
	_float fSizeY = XMVectorGetX(XMVector3Length(WorldMatrix.r[1]));

	RECT rcUI;
	SetRect(&rcUI,
		m_vPosition.x - fSizeX * 0.5f,
		m_vPosition.y - fSizeY * 0.5f,
		m_vPosition.x + fSizeX * 0.5f,
		m_vPosition.y + fSizeY * 0.5f);

	if (PtInRect(&rcUI, ptMouse))
	{
		m_pFocus->Late_Tick(TimeDelta);
		pInven->Set_On_Mouse(1);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CArmorCore::Render()
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;

	if (FAILED(m_pBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CArmorCore::Upgrade(class CInven* pInven, CDeffence* pDeffence)
{
	if (3 <= m_inumLevel)
		return;

	switch (m_inumLevel)
	{
	case 0:
		if (false == pInven->Get_Currency()->Pay_Currency(1000))
			return;
		break;

	case 1:
		if (false == pInven->Get_Currency()->Pay_Currency(5000))
			return;
		break;

	case 2:
		if (false == pInven->Get_Currency()->Pay_Currency(10000))
			return;
		break;
	}

	m_pCurrentTexture = m_pTextures[++m_inumLevel];
	if (3 == m_inumLevel)
	{
		_int iDeffence = pDeffence->Get_Deffence();
		pDeffence->Set_Deffence(iDeffence + 10);
	}
	else
	{
		_int iDeffence = pDeffence->Get_Deffence();
		pDeffence->Set_Deffence(iDeffence + 5);
	}
}

const _uint& CArmorCore::Get_NeedSouls()
{
	switch (m_inumLevel)
	{
	case 0:
		return 1000;

	case 1:
		return 5000;

	case 2:
		return 10000;
	}
}

HRESULT CArmorCore::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"VIBuffer_Rect", L"Com_Buffer",
		(CComponent**)&m_pBufferCom, m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_VtxTex", L"Com_Shader",
		(CComponent**)&m_pShaderCom, m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Renderer",
		(CComponent**)&m_pRendererCom, m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_UI_ArmorCore0", L"Texture_ArmorCore0",
		(CComponent**)&m_pTextures[0], m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_UI_ArmorCore1", L"Texture_ArmorCore1",
		(CComponent**)&m_pTextures[1], m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_UI_ArmorCore2", L"Texture_ArmorCore2",
		(CComponent**)&m_pTextures[2], m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_UI_ArmorCore3", L"Texture_ArmorCore3",
		(CComponent**)&m_pTextures[3], m_pOwner)))
		return E_FAIL;

	return S_OK;
}

HRESULT CArmorCore::Set_ShaderResources()
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

	if (FAILED(m_pCurrentTexture->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pCurrentTexture->Bind_ShaderResources(m_pShaderCom, "g_AlphaTexture")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CArmorCore* CArmorCore::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArmorCore* pInstance = new CArmorCore(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CArmorCore");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CArmorCore* CArmorCore::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CArmorCore* pInstance = new CArmorCore(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CArmorCore");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CArmorCore::Free()
{
	Safe_Release(m_pFocus);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);

	for (auto& pTexture : m_pTextures)
		Safe_Release(pTexture);

	Safe_Release(m_pRendererCom);

	CGameObjectUI::Free();
}
