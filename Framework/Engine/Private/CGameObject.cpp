#include "CGameObject.h"
#include "CGameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComposite(pDevice, pContext)
{
}

CGameObject::CGameObject(const CGameObject& rhs)
	:CComposite(rhs)
{
}

HRESULT CGameObject::Initialize_Prototype()
{
	if (FAILED(CComposite::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	if (FAILED(CComposite::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CGameObject::Tick(_double TimeDelta)
{
}

void CGameObject::Late_Tick(_double TimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::Free()
{
	CComposite::Free();
}
