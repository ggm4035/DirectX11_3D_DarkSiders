
#include "CFollow.h"

#include "CGameObject3D.h"
#include "CBlackBoard.h"
#include "CTransform.h"
#include "CModel.h"

CFollow::CFollow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CFollow::CFollow(const CFollow& rhs)
	: CBehavior(rhs)
{
}

HRESULT CFollow::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

HRESULT CFollow::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	CGameObject3D* pTarget = { nullptr };
	m_pBlackBoard->Get_Type(L"pTarget", pTarget);
	if (nullptr == pTarget)
		return E_FAIL;

	_vector vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPosition = pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_vector vDirection = XMVector3Normalize(vTargetPosition - vPosition);

	m_pTransform->Go_OnNavigation(vDirection, TimeDelta * 0.9f);
	m_pModel->Change_Animation("Run");

	return BEHAVIOR_RUNNING;
}

CFollow* CFollow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFollow* pInstance = new CFollow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CFollow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFollow* CFollow::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CFollow* pInstance = new CFollow(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CFollow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFollow::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}
	CBehavior::Free();
}
