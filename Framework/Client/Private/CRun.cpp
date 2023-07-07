#include "stdafx.h"
#include "CRun.h"

#include "CGameObject3D.h"
#include "CTransform.h"

CRun::CRun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CRun::CRun(const CRun& rhs)
	:CBehavior(rhs)
{
}

HRESULT CRun::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRun::Tick(_fvector vDirection, const _double& TimeDelta)
{
	CGameObject3D* pGameObject = dynamic_cast<CGameObject3D*>(m_pOwner);
	CTransform* pTransform = pGameObject->Get_Transform();

	pTransform->Go(vDirection, TimeDelta);

	return S_OK;
}

CRun* CRun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRun* pInstance = new CRun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRun* CRun::Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg)
{
	CRun* pInstance = new CRun(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CRun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRun::Free()
{
	CBehavior::Free();
}
