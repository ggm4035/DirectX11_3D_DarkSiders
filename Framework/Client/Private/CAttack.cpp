
#include "CAttack.h"

#include "CGameObject3D.h"
#include "CBlackBoard.h"
#include "CTransform.h"
#include "CModel.h"

CAttack::CAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CAttack::CAttack(const CAttack& rhs)
	: CBehavior(rhs)
{
}

HRESULT CAttack::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

HRESULT CAttack::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	CGameObject3D* pTarget = { nullptr };
	m_pBlackBoard->Get_Type(L"pTarget", pTarget);

	if (true == m_isFirst)
	{
		_vector vTargetPosition = pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);

		_vector vDirection = XMVector3Normalize(vTargetPosition - vPosition);
		_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_LOOK));

		_float f = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDirection, vLook))));
		if (6.f < f)
		{
			m_pTransform->Set_On_Navigation(true);
			m_pTransform->Go(vDirection, TimeDelta);
			return BEHAVIOR_RUNNING;
		}

		_uint iRand = rand() % 3;

		switch (iRand)
		{
		case 0:
			m_pModel->Change_Animation("Attack_1");
			break;

		case 1:
			m_pModel->Change_Animation("Attack_2");
			break;

		case 2:
			m_pModel->Change_Animation("Attack_3");
			break;
		}
		m_isFirst = false;
	}

	if (true == m_pModel->isLoopAnimation() ||
		true == m_pModel->isFinishedAnimation())
	{
		m_isFirst = true;
		m_pTransform->Set_On_Navigation(true);
		m_pModel->Change_Animation("Idle");
		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

CAttack* CAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttack* pInstance = new CAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAttack_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAttack* CAttack::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CAttack* pInstance = new CAttack(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAttack_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAttack::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
