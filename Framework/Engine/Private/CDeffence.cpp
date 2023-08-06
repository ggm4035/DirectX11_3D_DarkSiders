#include "CDeffence.h"

CDeffence::CDeffence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CDeffence::CDeffence(const CDeffence& rhs)
	: CComposite(rhs)
{
}

HRESULT CDeffence::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComposite::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		DEFFENCEDESC Desc = *static_cast<DEFFENCEDESC*>(pArg);
		m_iDeffence = Desc.iDeffence;
	}

	return S_OK;
}

CDeffence* CDeffence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDeffence* pInstance = new CDeffence(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDeffence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDeffence* CDeffence::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CDeffence* pInstance = new CDeffence(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CDeffence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDeffence::Free()
{
	CComposite::Free();
}
