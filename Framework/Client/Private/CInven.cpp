#include "CInven.h"

#include "CGameInstance.h"
#include "CCurrency.h"
#include "CUI_Rect.h"
#include "CAttackCore.h"
#include "CArmorCore.h"
#include "CHealthCore.h"

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
	for (auto& Data : m_isOnMouse)
		Data = false;

	if (false == m_isPopEnd)
		Activation(TimeDelta);

	Tick_UI(TimeDelta);

	if (false == m_isEnable)
		return;
}

void CInven::AfterFrustumTick(const _double& TimeDelta)
{
	m_pAttackCore->Late_Tick(this, TimeDelta);
	m_pArmorCore->Late_Tick(this, TimeDelta);
	m_pHealthCore->Late_Tick(this, TimeDelta);

	m_pCharBox->Late_Tick(TimeDelta);
	m_pPlayerIcon->Late_Tick(TimeDelta);
	m_pAttackDS->Late_Tick(TimeDelta);
	m_pDeffenceDS->Late_Tick(TimeDelta);
	m_pHealthDS->Late_Tick(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

void CInven::Late_Tick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (true == m_isOnMouse[0])
	{
		if (pGameInstance->Key_Down(DIK_X))
			m_pAttackCore->Upgrade(this, m_pPlayerAttack);
	}

	if (true == m_isOnMouse[1])
	{
		if (pGameInstance->Key_Down(DIK_X))
			m_pArmorCore->Upgrade(this, m_pPlayerDeffence);
	}

	if (true == m_isOnMouse[2])
	{
		if (pGameInstance->Key_Down(DIK_X))
			m_pHealthCore->Upgrade(this, m_pPlayerHealth);
	}

	Safe_Release(pGameInstance);
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

	_float2 vPosition = { 0.f, 0.f };
	wstring wstrText = L"";

	if (true == m_isOnMouse[0])
	{
		vPosition = _float2(m_vPosition.x, m_vPosition.y);
		vPosition.x -= 180.f;
		vPosition.y -= 5.f;

		wstrText = L"진노석 파편을 모을수록 \n워의 공격력이 증가합니다.";
		if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
			XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.5f)))
			return E_FAIL;

		vPosition = _float2(m_vPosition.x, m_vPosition.y);
		vPosition.x -= 185.f;
		vPosition.y += 80.f;
		wstrText = to_wstring(m_pAttackCore->Get_NeedSouls()) + L" 소울";
		if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
			XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.5f)))
			return E_FAIL;

		vPosition = _float2(m_vPosition.x, m_vPosition.y);
		if (m_pCurrency->Get_Currency() >= m_pAttackCore->Get_NeedSouls())
		{
			wstrText = L"업그레이드 : X";
			vPosition.x += 25.f;
			vPosition.y += 80.f;
		}
		else
		{
			wstrText = L"소울이 부족합니다.";
			vPosition.x -= 5.f;
			vPosition.y += 80.f;
		}

		if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
			XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.5f)))
			return E_FAIL;
	}

	if (true == m_isOnMouse[1])
	{
		vPosition = _float2(m_vPosition.x, m_vPosition.y);
		vPosition.x -= 180.f;
		vPosition.y -= 5.f;

		wstrText = L"심연의 갑옷 파편을 모을수록 \n워의 방어력이 증가합니다.";
		if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
			XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.5f)))
			return E_FAIL;

		vPosition = _float2(m_vPosition.x, m_vPosition.y);
		vPosition.x -= 185.f;
		vPosition.y += 80.f;
		wstrText = to_wstring(m_pArmorCore->Get_NeedSouls()) + L" 소울";
		if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
			XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.5f)))
			return E_FAIL;

		vPosition = _float2(m_vPosition.x, m_vPosition.y);
		if (m_pCurrency->Get_Currency() >= m_pArmorCore->Get_NeedSouls())
		{
			wstrText = L"업그레이드 : X";
			vPosition.x += 25.f;
			vPosition.y += 80.f;
		}
		else
		{
			wstrText = L"소울이 부족합니다.";
			vPosition.x -= 5.f;
			vPosition.y += 80.f;
		}

		if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
			XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.5f)))
			return E_FAIL;
	}

	if (true == m_isOnMouse[2])
	{
		vPosition = _float2(m_vPosition.x, m_vPosition.y);
		vPosition.x -= 180.f;
		vPosition.y -= 5.f;

		wstrText = L"체력 파편을 모을수록 \n워의 체력이 증가합니다.";
		if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
			XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.5f)))
			return E_FAIL;

		vPosition = _float2(m_vPosition.x, m_vPosition.y);
		vPosition.x -= 185.f;
		vPosition.y += 80.f;
		wstrText = to_wstring(m_pHealthCore->Get_NeedSouls()) + L" 소울";
		if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
			XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.5f)))
			return E_FAIL;

		vPosition = _float2(m_vPosition.x, m_vPosition.y);
		if (m_pCurrency->Get_Currency() >= m_pHealthCore->Get_NeedSouls())
		{
			wstrText = L"업그레이드 : X";
			vPosition.x += 25.f;
			vPosition.y += 80.f;
		}
		else
		{
			wstrText = L"소울이 부족합니다.";
			vPosition.x -= 5.f;
			vPosition.y += 80.f;
		}

		if (FAILED(pGameInstance->Render_Font(L"Font_135", wstrText, vPosition,
			XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(), 0.5f)))
			return E_FAIL;
	}

	vPosition = _float2(m_vPosition.x, m_vPosition.y);
	vPosition.x -= 15.f;
	vPosition.y += 185.f;

	wstrText = to_wstring(m_pPlayerAttack->Get_Damage());
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

	CAttackCore::UIDESC Desc;
	Desc.m_fX = 0.f;
	Desc.m_fY = 0.f;
	Desc.m_fDepth = 0.f;
	Desc.m_fSizeX = 150.f;
	Desc.m_fSizeY = 150.f;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Attack_Core", L"Attack_Core",
		(CComponent**)&m_pAttackCore, nullptr, &Desc)))
		return E_FAIL;

	Desc.m_fX = 0.f;
	Desc.m_fY = 0.f;
	Desc.m_fDepth = 0.f;
	Desc.m_fSizeX = 150.f;
	Desc.m_fSizeY = 150.f;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Armor_Core", L"Armor_Core",
		(CComponent**)&m_pArmorCore, nullptr, &Desc)))
		return E_FAIL;

	Desc.m_fX = 0.f;
	Desc.m_fY = 0.f;
	Desc.m_fDepth = 0.f;
	Desc.m_fSizeX = 150.f;
	Desc.m_fSizeY = 150.f;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Health_Core", L"Health_Core",
		(CComponent**)&m_pHealthCore, nullptr, &Desc)))
		return E_FAIL;

	CUI_Rect::UIRECTDESC UIDesc;
	UIDesc.m_fX = 100.f;
	UIDesc.m_fY = 222.f;
	UIDesc.m_fSizeX = 180.f;
	UIDesc.m_fSizeY = 180.f;
	UIDesc.m_fDepth = 0.f;
	UIDesc.wstrTextureTag = L"Texture_UI_War";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.iPassNum = 10;

	if (FAILED(Add_Component(LEVEL_STATIC, L"UI_Rect", L"UI_War",
		(CComponent**)&m_pPlayerIcon, nullptr, &UIDesc)))
		return E_FAIL;

	UIDesc.m_fX = 0.f;
	UIDesc.m_fY = 0.f;
	UIDesc.m_fSizeX = 370.f;
	UIDesc.m_fSizeY = 150.f;
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
	UIDesc.iPassNum = 9;

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
	UIDesc.iPassNum = 9;

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
	UIDesc.iPassNum = 9;

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
	vPosition.x -= 160.f;
	vPosition.y -= 200.f;
	vPosition.z = 0.f;
	m_pAttackCore->Tick(vPosition);

	vPosition = m_vPosition;
	vPosition.x -= 20.f;
	vPosition.y -= 200.f;
	vPosition.z = 0.f;
	m_pArmorCore->Tick(vPosition);

	vPosition = m_vPosition;
	vPosition.x += 120.f;
	vPosition.y -= 200.f;
	vPosition.z = 0.f;
	m_pHealthCore->Tick(vPosition);

	vPosition = m_vPosition;
	vPosition.x -= 120.f;
	vPosition.y += 200.f;
	vPosition.z = 0.f;
	m_pPlayerIcon->Set_Position(vPosition);

	vPosition = m_vPosition;
	vPosition.x -= 15.f;
	vPosition.y += 50.f;
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

	Safe_Release(m_pAttackCore);
	Safe_Release(m_pArmorCore);
	Safe_Release(m_pHealthCore);

	Safe_Release(m_pPlayerIcon);
	Safe_Release(m_pCharBox);
	Safe_Release(m_pAttackDS);
	Safe_Release(m_pDeffenceDS);
	Safe_Release(m_pHealthDS);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObjectUI::Free();
}
