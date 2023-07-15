#include "stdafx.h"
#include "CRun.h"

#include "CGameObject3D.h"
#include "CBlackBoard.h"
#include "CTransform.h"
#include "CModel.h"

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

	CGameObject3D* pGameObject = dynamic_cast<CGameObject3D*>(m_pOwner);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_pTransform = pGameObject->Get_Transform();
	if (nullptr == m_pTransform)
		return E_FAIL;
	Safe_AddRef(m_pTransform);

	m_pModel = dynamic_cast<CModel*>(pGameObject->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	return S_OK;
}

HRESULT CRun::Tick(const _double& TimeDelta)
{
	m_fTimeAcc += TimeDelta;

	_float3 vDirection;
	m_pBlackBoard->Get_Type<_float3>(L"vDirection", vDirection);

	m_pTransform->Go_OnNavigation(XMLoadFloat3(&vDirection), TimeDelta * 0.5f);
	m_pModel->Change_Animation("Run");

	if (m_fTimeAcc >= m_fLimit)
	{
		m_fTimeAcc = 0.f;
		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
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
