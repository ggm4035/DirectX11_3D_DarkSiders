#include "CMove.h"

#include "CGameObject3D.h"
#include "CBlackBoard.h"
#include "CTransform.h"
#include "CModel.h"

CMove::CMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CMove::CMove(const CMove& rhs)
	: CBehavior(rhs)
{
}

HRESULT CMove::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

	return S_OK;
}

HRESULT CMove::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	m_fTimeAcc += TimeDelta;

	_float3 vDirection;
	m_pBlackBoard->Get_Type(L"vDirection", vDirection);
	m_pTransform->Go(XMLoadFloat3(&vDirection), TimeDelta * m_fMoveForce);

	if (true == m_isUseTimer)
	{
		if (m_fLimit < m_fTimeAcc)
		{
			m_fTimeAcc = 0.f;
			return BEHAVIOR_SUCCESS;
		}

		return BEHAVIOR_RUNNING;
	}

	return BEHAVIOR_SUCCESS;
}

CMove* CMove::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMove* pInstance = new CMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMove");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMove* CMove::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CMove* pInstance = new CMove(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CMove");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMove::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
	}
	CBehavior::Free();
}
