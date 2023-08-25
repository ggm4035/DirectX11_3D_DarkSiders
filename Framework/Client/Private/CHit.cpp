#include "CHit.h"

#include "CGameInstance.h"
#include "CBlackBoard.h"

CHit::CHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CHit::CHit(const CHit& rhs)
	: CBehavior(rhs)
{
}

HRESULT CHit::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
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
			CHealth* pHealth = { nullptr };
			pBlackBoard->Get_Type(L"pHealth", pHealth);
			if (nullptr == pHealth)
				return false;

			return !pHealth->isDead();
		});

	return S_OK;
}

HRESULT CHit::Tick(const _double& TimeDelta)
{
	CGameObject3D* pTarget = { nullptr };
	CHealth* pHealth = { nullptr };

	m_pBlackBoard->Get_Type(L"pHealth", pHealth);
	m_pBlackBoard->Get_Type(L"pTarget", pTarget);

	switch (pHealth->Get_Current_HitState())
	{
	case CHealth::HIT_NONE:
		return BEHAVIOR_FAIL;

	case CHealth::HIT_ENTER:
		m_pTransform->Set_On_Navigation(true);

		CGameInstance::GetInstance()->Play_Sound(m_wstrSoundTag.c_str(), CSound_Manager::SOUND_ENEMY, 0.3f, true);

		if(true == m_isLook)
			m_pTransform->LookAt(pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));

		if(true == m_isImpact)
			m_pModel->Change_Animation("Impact");

		pHealth->Set_HitState(CHealth::HIT_STAY);

		return BEHAVIOR_SUCCESS;

	case CHealth::HIT_STAY:
		if (true == m_pModel->isFinishedAnimation())
		{
			pHealth->Set_HitState(CHealth::HIT_NONE);
			return BEHAVIOR_FAIL;
		}

		return BEHAVIOR_RUNNING;
	}

	return BEHAVIOR_FAIL;
}

CHit* CHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHit* pInstance = new CHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHit* CHit::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CHit* pInstance = new CHit(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CHit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHit::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
