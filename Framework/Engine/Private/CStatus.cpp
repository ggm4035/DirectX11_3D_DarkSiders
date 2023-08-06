#include "CStatus.h"

#include "CHealth.h"
#include "CAttack.h"
#include "CDeffence.h"

CStatus::CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CStatus::CStatus(const CStatus& rhs)
	: CComposite(rhs)
{
}

HRESULT CStatus::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComposite::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;
	
	return S_OK;
}

CStatus* CStatus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStatus* pInstance = new CStatus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CStatus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CStatus* CStatus::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CStatus* pInstance = new CStatus(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CStatus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatus::Free()
{
	CComposite::Free();
}
