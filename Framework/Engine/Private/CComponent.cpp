#include "CComponent.h"

#include "CComponent_Manager.h"
#include "CObject_Manager.h"

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

HRESULT CComponent::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	m_iLevelIndex = iLevelIndex;
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
	: CComponent(rhs)
{
}

HRESULT CComposite::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

CComponent* CComposite::Get_Component(const wstring& wstrComponentTag)
{
	auto iter = m_umapComponents.find(wstrComponentTag);

	if (iter == m_umapComponents.end())
		return nullptr;

	return iter->second;
}

HRESULT CComposite::Add_Component(const _uint& iLevelIndex, const wstring& PrototypeTag, const wstring& ComponentTag,
	CComponent** ppOut, CComponent* pOwner, void* pArg)
{
	CComponent* pComponent = CComponent_Manager::GetInstance()->
		Clone_Component(iLevelIndex, PrototypeTag, pOwner, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	auto& iter = m_umapComponents.find(ComponentTag);
	if (iter != m_umapComponents.end())
		return E_FAIL;

	m_umapComponents.emplace(ComponentTag, pComponent);
	*ppOut = pComponent;
	Safe_AddRef(pComponent);

	pComponent->Set_Tag(ComponentTag);

	return S_OK;
}

void CComposite::Free()
{
	for (auto& Pair : m_umapComponents)
		Safe_Release(Pair.second);
	m_umapComponents.clear();

	CComponent::Free();
}
