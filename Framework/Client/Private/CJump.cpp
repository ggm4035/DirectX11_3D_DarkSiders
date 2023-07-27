#include "CJump.h"

#include "CGameObject3D.h"
#include "CBlackBoard.h"
#include "CTransform.h"
#include "CModel.h"

CJump::CJump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CJump::CJump(const CJump& rhs)
	: CBehavior(rhs)
{
}

HRESULT CJump::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

HRESULT CJump::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	if (false == m_pTransform->Jump(m_fJumpForce, TimeDelta))
		return BEHAVIOR_FAIL;

	return BEHAVIOR_RUNNING;
}

CJump* CJump::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJump* pInstance = new CJump(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CJump");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CJump* CJump::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CJump* pInstance = new CJump(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CJump");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJump::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
	}
	CBehavior::Free();
}
