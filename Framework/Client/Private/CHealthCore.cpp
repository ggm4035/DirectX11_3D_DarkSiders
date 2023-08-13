#include "CHealthCore.h"

#include "CGameInstance.h"
#include "CCurrency.h"
#include "CUI_Rect.h"
#include "CInven.h"
#include "CAttack.h"

CHealthCore::CHealthCore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObjectUI(pDevice, pContext)
{
}

CHealthCore::CHealthCore(const CHealthCore& rhs)
	: CGameObjectUI(rhs)
{
}

HRESULT CHealthCore::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

void CHealthCore::Tick(const _float3& _vPosition)
{
	Set_Position(_vPosition);
	_float3 vPosition = m_vPosition;

	vPosition.x += 1.f;
	vPosition.z = 0.03f;
	m_pFocus->Set_Position(vPosition);
}

void CHealthCore::Late_Tick(CInven* pInven, const _double& TimeDelta)
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
		pInven->Set_On_Mouse(2);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHealthCore::Render()
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;

	if (FAILED(m_pBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CHealthCore::Upgrade(class CInven* pInven, CHealth* pHealth)
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
		_int iHealth = pHealth->Get_MaxHP();
		pHealth->Set_Max_Hp(iHealth + 200);
		pHealth->Heal(iHealth);
	}
	else
	{
		_int iHealth = pHealth->Get_MaxHP();
		pHealth->Set_Max_Hp(iHealth + 100);
		pHealth->Heal(iHealth);
	}
}

const _uint& CHealthCore::Get_NeedSouls()
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

	return 0;
}

HRESULT CHealthCore::Add_Components()
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

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_UI_HealthCore0", L"Texture_HealthCore0",
		(CComponent**)&m_pTextures[0], m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_UI_HealthCore1", L"Texture_HealthCore1",
		(CComponent**)&m_pTextures[1], m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_UI_HealthCore2", L"Texture_HealthCore2",
		(CComponent**)&m_pTextures[2], m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_UI_HealthCore3", L"Texture_HealthCore3",
		(CComponent**)&m_pTextures[3], m_pOwner)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHealthCore::Set_ShaderResources()
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

CHealthCore* CHealthCore::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHealthCore* pInstance = new CHealthCore(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHealthCore");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CHealthCore* CHealthCore::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CHealthCore* pInstance = new CHealthCore(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CHealthCore");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHealthCore::Free()
{
	Safe_Release(m_pFocus);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);

	for (auto& pTexture : m_pTextures)
		Safe_Release(pTexture);

	Safe_Release(m_pRendererCom);

	CGameObjectUI::Free();
}
