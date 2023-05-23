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

HRESULT CGameObject::Add_Component(_uint iNumLevel, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CComponent* pComponent = pGameInstance->Clone_Component(iNumLevel, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	auto& iter = find_if(m_pComponents.begin(), m_pComponents.end(), CTag_Finder(pComponentTag));
	if (iter != m_pComponents.end())
		return E_FAIL;

	m_pComponents.emplace(pComponentTag, pComponent);
	*ppOut = pComponent;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CGameObject::Free()
{
	CComposite::Free();
}
