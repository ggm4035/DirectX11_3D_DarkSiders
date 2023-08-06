#include "CAttack.h"

CAttack::CAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CAttack::CAttack(const CAttack& rhs)
	: CComposite(rhs)
{
}

HRESULT CAttack::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComposite::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		ATTACKDESC Desc = *static_cast<ATTACKDESC*>(pArg);

		m_iDamage = Desc.iDamage;
		m_isIgnoreDeffence = Desc.isIgnoreDeffence;
	}

	return S_OK;
}

CAttack* CAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttack* pInstance = new CAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAttack* CAttack::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CAttack* pInstance = new CAttack(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAttack::Free()
{
	CComposite::Free();
}
