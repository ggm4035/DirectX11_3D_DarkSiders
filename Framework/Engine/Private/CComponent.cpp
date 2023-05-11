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

CComponent::~CComponent()
{
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

CComposite::~CComposite()
{
}

void CComposite::Update_Component(_double TimeDelta)
{
	for (auto& Component : m_vecComponents)
		Component->Update_Component(TimeDelta);
}

HRESULT CComposite::Render()
{
	HRESULT result{};

	for (auto& Component : m_vecComponents)
	{
		result = Component->Render();
		if (E_FAIL == result)
			return E_FAIL;
	}
	return S_OK;
}

void CComposite::Free()
{
	m_vecComponents.clear();
	m_vecComponents.shrink_to_fit();

	CComponent::Free();
}
