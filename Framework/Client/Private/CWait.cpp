
#include "CWait.h"

#include "CGameObject3D.h"
#include "CBlackBoard.h"
#include "CTransform.h"
#include "CModel.h"

CWait::CWait(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CWait::CWait(const CWait& rhs)
	: CBehavior(rhs)
{
}

HRESULT CWait::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	CGameObject3D* pGameObject = dynamic_cast<CGameObject3D*>(m_pOwner);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_pModel = dynamic_cast<CModel*>(pGameObject->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	return S_OK;
}

HRESULT CWait::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	m_fTimeAcc += TimeDelta;
	m_pModel->Change_Animation("Idle");

	if (m_fTimeAcc >= m_fLimit)
	{
		m_fTimeAcc = 0.f;
		return BEHAVIOR_SUCCESS;
	}
	
	return BEHAVIOR_RUNNING;
}

CWait* CWait::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWait* pInstance = new CWait(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWait");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CWait* CWait::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CWait* pInstance = new CWait(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CWait");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWait::Free()
{
	if(true == m_isCloned)
		Safe_Release(m_pModel);

	CBehavior::Free();
}
