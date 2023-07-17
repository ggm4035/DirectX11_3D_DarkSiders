
#include "CWalk.h"

#include "CGameObject3D.h"
#include "CBlackBoard.h"
#include "CTransform.h"
#include "CModel.h"

CWalk::CWalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CWalk::CWalk(const CWalk& rhs)
	: CBehavior(rhs)
{
}

HRESULT CWalk::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

HRESULT CWalk::Tick(const _double& TimeDelta)
{
	m_fTimeAcc += TimeDelta;

	_float3 vDirection;
	m_pBlackBoard->Get_Type<_float3>(L"vDirection", vDirection);

	m_pTransform->Go_OnNavigation(XMLoadFloat3(&vDirection), TimeDelta * 0.5f);
	m_pModel->Change_Animation("Walk");

	if (m_fTimeAcc >= m_fLimit)
	{
		m_fTimeAcc = 0.f;
		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

CWalk* CWalk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWalk* pInstance = new CWalk(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWalk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CWalk* CWalk::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CWalk* pInstance = new CWalk(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CWalk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWalk::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}
	CBehavior::Free();
}
