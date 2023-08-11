#include "CSlot.h"

#include "CGameInstance.h"
#include "CCurrency.h"
#include "CUI_Rect.h"
#include "CItem.h"

CSlot::CSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObjectUI(pDevice, pContext)
{
}

CSlot::CSlot(const CSlot& rhs)
	: CGameObjectUI(rhs)
{
}

CItem* CSlot::Get_Item()
{
	if (nullptr == m_pItem)
		return nullptr;

	return m_pItem;
}

_bool CSlot::Set_Item(CItem* pItem)
{
	if (nullptr != m_pItem)
		return false;

	m_pItem = pItem;
	Safe_AddRef(m_pItem);

	return true;
}

HRESULT CSlot::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObjectUI::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		SLOTDESC Desc = *static_cast<SLOTDESC*>(pArg);

		if (FAILED(Add_Component(LEVEL_GAMEPLAY, Desc.wstrTextureTag, L"Texture_Slot",
			(CComponent**)&m_pTextureCom, m_pOwner)))
			return E_FAIL;

		_float fSize = { 0.f };

		if (wstring::npos != Desc.wstrTextureTag.find(L"Major"))
			fSize = 20.f;
		else
			fSize = -5.f;

		CUI_Rect::UIRECTDESC UIDesc;
		UIDesc.m_fX = 100.f;
		UIDesc.m_fY = 222.f;
		UIDesc.m_fSizeX = Desc.m_fSizeX + fSize;
		UIDesc.m_fSizeY = Desc.m_fSizeY + fSize;
		UIDesc.m_fDepth = 0.f;
		UIDesc.wstrTextureTag = L"Texture_UI_Focus";
		UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
		UIDesc.iPassNum = 11;

		if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UI_HealthDS",
			(CComponent**)&m_pFocus, nullptr, &UIDesc)))
			return E_FAIL;
	}

	return S_OK;
}

void CSlot::Tick(const _float3& _vPosition)
{
	m_vPosition = _vPosition;
	Set_Position(_vPosition);

	_float3 vPosition = m_vPosition;

	vPosition.x += 1.f;
	vPosition.z = 0.03f;
	m_pFocus->Set_Position(vPosition);
}

void CSlot::Late_Tick(const _double& TimeDelta)
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
		m_pFocus->Late_Tick(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CSlot::Render()
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;

	if (FAILED(m_pBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSlot::Add_Components()
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

	return S_OK;
}

HRESULT CSlot::Set_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_AlphaTexture")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CSlot* CSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlot* pInstance = new CSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CSlot* CSlot::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CSlot* pInstance = new CSlot(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSlot::Free()
{
	Safe_Release(m_pItem);
	Safe_Release(m_pFocus);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObjectUI::Free();
}
