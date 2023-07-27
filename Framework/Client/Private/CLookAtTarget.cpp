#include "CLookAtTarget.h"

#include "CGameObject3D.h"
#include "CGameInstance.h"
#include "CBlackBoard.h"

CLookAtTarget::CLookAtTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CLookAtTarget::CLookAtTarget(const CLookAtTarget& rhs)
	: CBehavior(rhs)
{
}

HRESULT CLookAtTarget::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

HRESULT CLookAtTarget::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	if (true == m_isUseTimer)
	{
		m_fTimeAcc += TimeDelta;

		if (m_fLimit <= m_fTimeAcc)
		{
			m_fTimeAcc = 0.f;
			return BEHAVIOR_FAIL;
		}
	}

	CGameObject3D* pTarget = { nullptr };
	m_pBlackBoard->Get_Type(L"pTarget", pTarget);

	_vector vTargetPosition = pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDirection = XMVector3Normalize(vTargetPosition - vPosition);
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_LOOK));

	_float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDirection, vLook))));

	m_pTransform->Set_On_Navigation(true);
	m_pTransform->Turn(vDirection, TimeDelta, 5.f);

	if (10.f > fDegree)
	{
		m_fTimeAcc = 0.f;
		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

CLookAtTarget* CLookAtTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLookAtTarget* pInstance = new CLookAtTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLookAtTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLookAtTarget* CLookAtTarget::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CLookAtTarget* pInstance = new CLookAtTarget(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CLookAtTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLookAtTarget::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
