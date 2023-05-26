#include "CComponent.h"

#include "CGameInstance.h"

/*=======================================*/
//				COMPONENT
/*=======================================*/

CComponent::CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent& rhs)
	: m_pDevice{ rhs.m_pDevice }
	, m_pContext{ rhs.m_pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
	return S_OK;
}

void CComponent::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

/*=======================================*/
//				COMPOSITE
/*=======================================*/

CComposite::CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CComposite::CComposite(const CComposite& rhs)
	:CComponent(rhs)
{
}

HRESULT CComposite::Initialize_Prototype()
{
	HRESULT result = CComponent::Initialize_Prototype();

	return result;
}

HRESULT CComposite::Initialize(void* pArg)
{
	HRESULT result = CComponent::Initialize(pArg);

	return result;
}

HRESULT CComposite::Add_Component(_uint iNumLevel, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CComponent* pComponent = pGameInstance->Clone_Component(iNumLevel, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	auto& iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));
	if (iter != m_Components.end())
		return E_FAIL;

	m_Components.insert({ pComponentTag, pComponent });
	*ppOut = pComponent;
	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);

	return S_OK;
}


void CComposite::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	CComponent::Free();
}
