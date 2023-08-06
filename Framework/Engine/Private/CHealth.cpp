#include "CHealth.h"

CHealth::CHealth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CHealth::CHealth(const CHealth& rhs)
	: CComposite(rhs)
{
}

HRESULT CHealth::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComposite::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		tagHealthDesc Desc = *static_cast<tagHealthDesc*>(pArg);
		m_iMaxHP = Desc.iMaxHP;
		m_iHP = Desc.iHP;
	}

	return S_OK;
}

CHealth* CHealth::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHealth* pInstance = new CHealth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHealth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHealth* CHealth::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CHealth* pInstance = new CHealth(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CHealth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHealth::Free()
{
	CComposite::Free();
}
