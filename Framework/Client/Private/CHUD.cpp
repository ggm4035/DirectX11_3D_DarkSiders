#include "CHUD.h"

#include "CGameInstance.h"
#include "CUI_Rect.h"
#include "CPlayer.h"
#include "CInven.h"
#include "CCurrency.h"
#include "CPlayerAction.h"

CHUD::CHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObjectUI(pDevice, pContext)
{
}

CHUD::CHUD(const CHUD& rhs)
	: CGameObjectUI(rhs)
{
}

HRESULT CHUD::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	m_pPlayer = dynamic_cast<CPlayer*>(CGameInstance::GetInstance()->Get_Player());
	if (nullptr == m_pPlayer)
		return E_FAIL;
	Safe_AddRef(m_pPlayer);

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CHUD::Tick(const _double& TimeDelta)
{
	for (auto UI : m_pPlayerUI)
		UI->Tick(TimeDelta);
}

void CHUD::Late_Tick(const _double& TimeDelta)
{
	for (auto UI : m_pPlayerUI)
		UI->Late_Tick(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHUD::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iCurrency = m_pPlayer->Get_Inven()->Get_Currency()->Get_Currency();

	wstring wstrCurrency = to_wstring(iCurrency);

	if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrCurrency, _float2(120.f, 220.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.5f)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CHUD::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Rect::UIRECTDESC UIDesc;

	UIDesc.m_fX = 250.f;
	UIDesc.m_fY = 110.f;
	UIDesc.m_fSizeX = 220.f;
	UIDesc.m_fSizeY = 40.f;
	UIDesc.m_fDepth = 0.01f;
	UIDesc.wstrTextureTag = L"Texture_UI_UnitFrame_Hp";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 2;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UnitFrameHp",
		(CComponent**)&m_pPlayerUI[0], nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 100.f;
	UIDesc.m_fY = 100.f;
	UIDesc.m_fSizeX = 100.f;
	UIDesc.m_fSizeY = 100.f;
	UIDesc.m_fDepth = 0.01f;
	UIDesc.wstrTextureTag = L"Texture_UI_UnitFrame_2";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 2;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UnitFrame2",
		(CComponent**)&m_pPlayerUI[1], nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 105.f;
	UIDesc.m_fY = 103.f;
	UIDesc.m_fSizeX = 75.f;
	UIDesc.m_fSizeY = 77.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_UnitFrame_1";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 2;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UnitFrame1",
		(CComponent**)&m_pPlayerUI[2], nullptr, &UIDesc)))
		return E_FAIL;

	CGameObject3D* pPlayer = pGameInstance->Get_Player();
	UIDesc.m_fX = 247.f;
	UIDesc.m_fY = 106.f;
	UIDesc.m_fSizeX = 185.f;
	UIDesc.m_fSizeY = 14.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_UnitFrame_HpBar";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 3;
	UIDesc.pHealth = dynamic_cast<CHealth*>(pPlayer->Get_Component(L"Com_Health"));

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UnitHpBar",
		(CComponent**)&m_pPlayerUI[3], nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 150.f;
	UIDesc.m_fY = 600.f;
	UIDesc.m_fSizeX = 184.f;
	UIDesc.m_fSizeY = 88.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_SkillFrame";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 2;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"SkillFrame",
		(CComponent**)&m_pPlayerUI[4], nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 150.f;
	UIDesc.m_fY = 590.f;
	UIDesc.m_fSizeX = 80.f;
	UIDesc.m_fSizeY = 80.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_Ability_Dash";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 2;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"Ability_Dash",
		(CComponent**)&m_pPlayerUI[5], nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 90.f;
	UIDesc.m_fY = 610.f;
	UIDesc.m_fSizeX = 80.f;
	UIDesc.m_fSizeY = 80.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_Ability_Leap";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 6;
	UIDesc.pCoolTime = static_cast<CPlayer*>(pGameInstance->Get_Player())->Get_Action()->Get_LeapCoolTimePtr();

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"Ability_Leap",
		(CComponent**)&m_pPlayerUI[6], nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 210.f;
	UIDesc.m_fY = 610.f;
	UIDesc.m_fSizeX = 80.f;
	UIDesc.m_fSizeY = 80.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_Ability_Wheel";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 6;
	UIDesc.pCoolTime = static_cast<CPlayer*>(pGameInstance->Get_Player())->Get_Action()->Get_WheelCoolTimePtr();

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"Ability_Wheel",
		(CComponent**)&m_pPlayerUI[7], nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 100.f;
	UIDesc.m_fY = 222.f;
	UIDesc.m_fSizeX = 80.f;
	UIDesc.m_fSizeY = 80.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_Currency_Soul";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 9;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"Currency_Soul",
		(CComponent**)&m_pPlayerUI[8], nullptr, &UIDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CHUD* CHUD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHUD* pInstance = new CHUD(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHUD");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CHUD* CHUD::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CHUD* pInstance = new CHUD(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CHUD");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHUD::Free()
{
	Safe_Release(m_pPlayer);

	for (auto& UI : m_pPlayerUI)
		Safe_Release(UI);

	Safe_Release(m_pRendererCom);

	CGameObjectUI::Free();
}
