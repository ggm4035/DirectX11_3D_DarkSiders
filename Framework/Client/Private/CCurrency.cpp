#include "CCurrency.h"

#include "CGameInstance.h"

CCurrency::CCurrency(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCurrency::CCurrency(const CCurrency& rhs)
	: CComponent(rhs)
{
}

HRESULT CCurrency::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CCurrency::Pay_Currency(const _uint& iValue)
{
	if (m_iCurrency < iValue)
		return false;

	m_iCurrency -= iValue;

	return true;
}

CCurrency* CCurrency::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCurrency* pInstance = new CCurrency(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCurrency");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCurrency* CCurrency::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CCurrency* pInstance = new CCurrency(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CCurrency");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCurrency::Free()
{
	CComponent::Free();
}
