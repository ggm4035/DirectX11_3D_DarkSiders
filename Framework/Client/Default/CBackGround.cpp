#include "stdafx.h"
#include "CBackGround.h"
#include "CRenderer.h"
#include "CGameInstance.h"

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CBackGround::CBackGround(const CBackGround& rhs)
	:CGameObject(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
}

void CBackGround::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CBackGround::Render()
{
	if (FAILED(CGameObject::Render()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CBackGround::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Renderer",
		L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect",
		L"Com_Buffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_Vtxtex",
		L"Com_Shader_Vtxtex", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackGround* pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackGround::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	CGameObject::Free();
}
