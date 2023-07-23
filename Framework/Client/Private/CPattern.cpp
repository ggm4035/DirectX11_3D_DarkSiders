
#include "CPattern.h"

#include "CBlackBoard.h"
#include "CGameObject3D.h"
#include "CModel.h"

CPattern::CPattern(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CPattern::CPattern(const CPattern& rhs)
	: CBehavior(rhs)
{
}

HRESULT CPattern::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

HRESULT CPattern::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	HRESULT hr = S_OK;

	switch (m_eCurState)
	{
	case Client::CPattern::STATE_NONE:
		m_pModel->Change_Animation(m_strStartAnimTag.c_str());
		m_eCurState = STATE_START;
		break;

	case Client::CPattern::STATE_START:
		if (true == m_pModel->isFinishedAnimation())
		{
			m_pModel->Change_Animation(m_strRunningAnimTag.c_str());
			m_eCurState = STATE_RUNING;
		}
		break;

	case Client::CPattern::STATE_RUNING:
		hr = m_BehaviorList.begin()->pBehavior->Tick(TimeDelta);
		if (hr != BEHAVIOR_RUNNING)
		{
			m_pModel->Change_Animation(m_strFinishAnimTag.c_str());
			m_eCurState = STATE_FINISH;
		}
		break;

	case Client::CPattern::STATE_FINISH:
		if (true == m_pModel->isFinishedAnimation())
		{
			_float* pTimeAcc = { nullptr };
			m_pBlackBoard->Get_Type(L"fTimeAcc", pTimeAcc);
			*pTimeAcc = 0.f;
			m_eCurState = STATE_NONE;
		}
		break;
	}

	return BEHAVIOR_RUNNING;
}

CPattern* CPattern::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPattern* pInstance = new CPattern(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPattern* CPattern::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPattern* pInstance = new CPattern(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPattern::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
