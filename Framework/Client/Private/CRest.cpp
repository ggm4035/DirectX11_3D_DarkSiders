
#include "CRest.h"

#include "CGameObject3D.h"
#include "CBlackBoard.h"
#include "CTransform.h"
#include "CModel.h"

CRest::CRest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CRest::CRest(const CRest& rhs)
	: CBehavior(rhs)
{
}

HRESULT CRest::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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
			_bool* pIsSpawn = { nullptr };

			pBlackBoard->Get_Type(L"isSpawn", pIsSpawn);

			return !(*pIsSpawn);
		});

	return S_OK;
}

HRESULT CRest::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
	{
		m_iterAnimTag = m_AnimTagList.begin();
		m_isFirst = true;
		return BEHAVIOR_FAIL;
	}

	if (0 == m_AnimTagList.size())
		return BEHAVIOR_FAIL;

	if (true == m_isFirst)
	{
		m_pModel->Change_Animation((*m_iterAnimTag).c_str());
		m_isFirst = false;
	}

	if (true == m_pModel->isFinishedAnimation())
	{
		m_pModel->Change_Animation((*m_iterAnimTag).c_str());
		++m_iterAnimTag;

		if (m_iterAnimTag == m_AnimTagList.end())
			m_iterAnimTag = m_AnimTagList.begin();
	}

	return BEHAVIOR_RUNNING;
}

CRest* CRest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRest* pInstance = new CRest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRest* CRest::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CRest* pInstance = new CRest(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CRest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRest::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}
	CBehavior::Free();
}
