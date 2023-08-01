
#include "CCursor.h"

#include "CGameInstance.h"

CCursor::CCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObjectUI(pDevice, pContext)
{
}

CCursor::CCursor(const CCursor& rhs)
	: CGameObjectUI(rhs)
{
}

HRESULT CCursor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCursor::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(CGameObjectUI::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CCursor::Tick(const _double& TimeDelta)
{
	if (false == m_isRender)
		return;

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_float3 vPosition = _float3((_float)ptMouse.x, (_float)ptMouse.y, 0.f);

	Set_Position(vPosition);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

void CCursor::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CCursor::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CCursor::Add_Components()
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

	if (FAILED(Add_Component(LEVEL_STATIC, L"Texture_UI_Cursor",
		L"Com_Texture", (CComponent**)&m_pTextureCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCursor::SetUp_ShaderResources()
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

	Safe_Release(pGameInstance);

	return S_OK;
}

CCursor* CCursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCursor* pInstance = new CCursor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCursor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCursor* CCursor::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CCursor* pInstance = new CCursor(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CCursor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCursor::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	CGameObjectUI::Free();
}
