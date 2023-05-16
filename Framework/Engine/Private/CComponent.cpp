#include "..\Public\CComponent.h"

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

void CComposite::Free()
{
	m_vecComponents.clear();
	m_vecComponents.shrink_to_fit();

	CComponent::Free();
}
