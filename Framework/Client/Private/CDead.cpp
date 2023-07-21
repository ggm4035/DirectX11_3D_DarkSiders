
#include "CDead.h"

#include "CBlackBoard.h"
#include "CGameObject3D.h"
#include "CModel.h"

CDead::CDead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CDead::CDead(const CDead& rhs)
	: CBehavior(rhs)
{
}

HRESULT CDead::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsDead = { nullptr };
			pBlackBoard->Get_Type(L"isDead", pIsDead);
			if (nullptr == pIsDead)
				return false;

			return *pIsDead;
		});

	return S_OK;
}

HRESULT CDead::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	if (true == m_isFirst)
	{
		m_pModel->Change_Animation("Dead");
		m_isFirst = false;
	}

	static_cast<CGameObject3D*>(m_pOwner)->Dead_Motion();

	if(true == m_pModel->isFinishedAnimation())
	{
		m_isFirst = true;
		_bool* pIsRemove = { nullptr };
		m_pBlackBoard->Get_Type(L"isRemove", pIsRemove);
		*pIsRemove = true;
		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

CDead* CDead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDead* pInstance = new CDead(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDead* CDead::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CDead* pInstance = new CDead(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CDead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDead::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
