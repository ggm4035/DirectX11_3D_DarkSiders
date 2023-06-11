#include "CComponent.h"

#include "CComponent_Manager.h"

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

HRESULT CComponent::Initialize(CComponent* pOwner, void* pArg)
{
	m_pOwner = pOwner;
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
	return S_OK;
}

HRESULT CComposite::Initialize(CComponent* pOwner, void* pArg)
{
	if(FAILED(CComponent::Initialize(pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

void CComposite::Tick(const _double& TimeDelta)
{
}

void CComposite::Late_Tick(_double TimeDelta)
{
}

HRESULT CComposite::Render()
{
	return S_OK;
}

HRESULT CComposite::Add_Component(_uint iNumLevel, wstring PrototypeTag, wstring ComponentTag, CComponent** ppOut, CComponent* pOwner, void* pArg)
{
 	CComponent* pComponent = CComponent_Manager::GetInstance()->
		Clone_Component(iNumLevel, PrototypeTag, pOwner, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	auto& iter = m_Components.find(ComponentTag);
	if (iter != m_Components.end())
		return E_FAIL;

	m_Components.emplace(ComponentTag, pComponent);
	*ppOut = pComponent;
	Safe_AddRef(pComponent);

	pComponent->Set_Tag(ComponentTag);

	return S_OK;
}

void CComposite::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	CComponent::Free();
}
