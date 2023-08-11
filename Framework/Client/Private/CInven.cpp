#include "CInven.h"

#include "CGameInstance.h"
#include "CCurrency.h"
#include "CUI_Rect.h"
#include "CSlot.h"

#include "CHealth.h"
#include "CAttack.h"
#include "CDeffence.h"

CInven::CInven(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObjectUI(pDevice, pContext)
{
}

CInven::CInven(const CInven& rhs)
	: CGameObjectUI(rhs)
{
}

HRESULT CInven::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	UIDESC UIDesc;
	UIDesc.m_fX = 1640.f;//1040.f;//
	UIDesc.m_fY = _float(g_iWinSizeY >> 1);
	UIDesc.m_fSizeX = 800.f;
	UIDesc.m_fSizeY = _float(g_iWinSizeY);
	UIDesc.m_fDepth = 0.05f;

	if (FAILED(CGameObjectUI::Initialize(iLevelIndex, pOwner, &UIDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		INVENDESC Desc = *static_cast<INVENDESC*>(pArg);
		m_pPlayerAttack = Desc.pAttack;
		m_pPlayerDeffence = Desc.pDeffence;
		m_pPlayerHealth = Desc.pHealth;

		Safe_AddRef(m_pPlayerAttack);
		Safe_AddRef(m_pPlayerDeffence);
		Safe_AddRef(m_pPlayerHealth);
	}

	return S_OK;
}

void CInven::Tick(const _double& TimeDelta)
{
	if (false == m_isPopEnd)
		Activation(TimeDelta);

	Tick_UI(TimeDelta);
	
	if (false == m_isEnable)
		return;


}

void CInven::AfterFrustumTick(const _double& TimeDelta)
{
	/* 구조 꼬여서 ui는 latetick... */
	m_pCharBox->Late_Tick(TimeDelta);
	m_pPlayerIcon->Late_Tick(TimeDelta);
	m_pAttackDS->Late_Tick(TimeDelta);
	m_pDeffenceDS->Late_Tick(TimeDelta);
	m_pHealthDS->Late_Tick(TimeDelta);

	m_pMajor->Late_Tick(TimeDelta);
	for (auto& pMinor : m_pMinors)
		pMinor->Late_Tick(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInven::Render()
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(9)))
		return E_FAIL;

	if (FAILED(m_pBufferCom->Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float2 vPosition = _float2(m_vPosition.x, m_vPosition.y);

	vPosition.x -= 15.f;
	vPosition.y += 185.f;

	wstring wstrText = to_wstring(m_pPlayerAttack->Get_Damage());
	if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
		XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.4f)))
		return E_FAIL;

	vPosition = _float2(m_vPosition.x, m_vPosition.y);

	vPosition.x -= 15.f;
	vPosition.y += 220.f;

	wstrText = to_wstring(m_pPlayerDeffence->Get_Deffence());
	if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
		XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.4f)))
		return E_FAIL;

	vPosition = _float2(m_vPosition.x, m_vPosition.y);

	vPosition.x -= 15.f;
	vPosition.y += 255.f;

	wstrText = to_wstring(m_pPlayerHealth->Get_MaxHP());
	if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
		XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.4f)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CInven::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Currency", L"Currency",
		(CComponent**)&m_pCurrency, m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_Inven", L"Texture_Inven",
		(CComponent**)&m_pTextureCom, m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"VIBuffer_Rect", L"Com_Buffer",
		(CComponent**)&m_pBufferCom, m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_VtxTex", L"Com_Shader",
		(CComponent**)&m_pShaderCom, m_pOwner)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Renderer",
		(CComponent**)&m_pRendererCom, m_pOwner)))
		return E_FAIL;

	CSlot::SLOTDESC SlotDesc;
	SlotDesc.m_fX = 0.f;
	SlotDesc.m_fY = 0.f;
	SlotDesc.m_fDepth = 0.f;
	SlotDesc.m_fSizeX = 100.f;
	SlotDesc.m_fSizeY = 100.f;
	SlotDesc.wstrTextureTag = L"Texture_UI_CoreMajor";
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Slot", L"Core_Major",
		(CComponent**)&m_pMajor, m_pOwner, &SlotDesc)))
		return E_FAIL;

	SlotDesc.wstrTextureTag = L"Texture_UI_CoreMinor";
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Slot", L"Core_Minor1",
		(CComponent**)&m_pMinors[0], m_pOwner, &SlotDesc)))
		return E_FAIL;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Slot", L"Core_Minor2",
		(CComponent**)&m_pMinors[1], m_pOwner, &SlotDesc)))
		return E_FAIL;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Slot", L"Core_Minor3",
		(CComponent**)&m_pMinors[2], m_pOwner, &SlotDesc)))
		return E_FAIL;
	
	CUI_Rect::UIRECTDESC UIDesc;
	UIDesc.m_fX = 100.f;
	UIDesc.m_fY = 222.f;
	UIDesc.m_fSizeX = 180.f;
	UIDesc.m_fSizeY = 180.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_War";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 9;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UI_War",
		(CComponent**)&m_pPlayerIcon, nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 100.f;
	UIDesc.m_fY = 222.f;
	UIDesc.m_fSizeX = 400.f;
	UIDesc.m_fSizeY = 200.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_CharBox";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 10;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UI_CharBox",
		(CComponent**)&m_pCharBox, nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 100.f;
	UIDesc.m_fY = 222.f;
	UIDesc.m_fSizeX = 50.f;
	UIDesc.m_fSizeY = 50.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_AttackDS";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 10;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UI_AttackDS",
		(CComponent**)&m_pAttackDS, nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 100.f;
	UIDesc.m_fY = 222.f;
	UIDesc.m_fSizeX = 50.f;
	UIDesc.m_fSizeY = 50.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_DeffenceDS";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 10;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UI_DeffenceDS",
		(CComponent**)&m_pDeffenceDS, nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 100.f;
	UIDesc.m_fY = 222.f;
	UIDesc.m_fSizeX = 50.f;
	UIDesc.m_fSizeY = 50.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_HealthDS";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 10;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UI_HealthDS",
		(CComponent**)&m_pHealthDS, nullptr, &UIDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInven::Set_ShaderResources()
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

void CInven::Tick_UI(const _double& TimeDelta)
{
	_float3 vPosition = m_vPosition;

	vPosition.x -= 120.f;
	vPosition.y += 200.f;
	vPosition.z = 0.f;
	m_pPlayerIcon->Set_Position(vPosition);

	vPosition = m_vPosition;
	vPosition.x -= 15.f;
	vPosition.y += 200.f;
	vPosition.z = 0.01f;
	m_pCharBox->Set_Position(vPosition);

	vPosition = m_vPosition;
	vPosition.x -= 35.f;
	vPosition.y += 195.f;
	vPosition.z = 0.01f;
	m_pAttackDS->Set_Position(vPosition);

	vPosition = m_vPosition;
	vPosition.x -= 35.f;
	vPosition.y += 230.f;
	vPosition.z = 0.01f;
	m_pDeffenceDS->Set_Position(vPosition);

	vPosition = m_vPosition;
	vPosition.x -= 35.f;
	vPosition.y += 265.f;
	vPosition.z = 0.01f;
	m_pHealthDS->Set_Position(vPosition);

	vPosition = m_vPosition;
	vPosition.x -= 10.f;
	vPosition.y -= 50.f;
	vPosition.z = 0.02f;
	m_pMajor->Tick(vPosition);

	vPosition = m_vPosition;
	vPosition.x -= 130.f;
	vPosition.y += 50.f;
	vPosition.z = 0.02f;
	m_pMinors[0]->Tick(vPosition);

	vPosition = m_vPosition;
	vPosition.x -= 10.f;
	vPosition.y += 50.f;
	vPosition.z = 0.02f;
	m_pMinors[1]->Tick(vPosition);

	vPosition = m_vPosition;
	vPosition.x += 110.f;
	vPosition.y += 50.f;
	vPosition.z = 0.02f;
	m_pMinors[2]->Tick(vPosition);
}

void CInven::Activation(const _double& TimeDelta)
{
	_float3 vPos = m_vPosition;

	if (true == m_isEnable)
		m_vDestX = 1040.f;
	else
		m_vDestX = 1640.f;

	if (20.f < fabs(m_vDestX - vPos.x))
	{
		vPos.x = lerp(vPos.x, m_vDestX, TimeDelta * 2.5f);
		Set_Position(vPos);
	}
	else
		m_isPopEnd = true;
}

CInven* CInven::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInven* pInstance = new CInven(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CInven");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CInven* CInven::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CInven* pInstance = new CInven(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CInven");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInven::Free()
{
	Safe_Release(m_pCurrency);

	Safe_Release(m_pPlayerAttack);
	Safe_Release(m_pPlayerDeffence);
	Safe_Release(m_pPlayerHealth);

	Safe_Release(m_pPlayerIcon);
	Safe_Release(m_pCharBox);
	Safe_Release(m_pAttackDS);
	Safe_Release(m_pDeffenceDS);
	Safe_Release(m_pHealthDS);

	Safe_Release(m_pMajor);
	for (auto& pMinor : m_pMinors)
		Safe_Release(pMinor);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObjectUI::Free();
}
