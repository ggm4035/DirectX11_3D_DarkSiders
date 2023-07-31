
#include "CPlayerMove.h"

#include "CPlayerAction.h"
#include "CPlayer.h"
#include "CModel.h"

CPlayerMove::CPlayerMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CPlayerMove::CPlayerMove(const CPlayerMove& rhs)
	: CBehavior(rhs)
{
}

HRESULT CPlayerMove::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pOwner);
	if (nullptr == pPlayer)
		return E_FAIL;

	m_pCamera = dynamic_cast<CGameObject3D*>(pPlayer->Get_Parts(L"Camera_Free"));
	if (nullptr == m_pCamera)
		return E_FAIL;
	Safe_AddRef(m_pCamera);

	m_pTransform = pPlayer->Get_Transform();
	if (nullptr == m_pTransform)
		return E_FAIL;
	Safe_AddRef(m_pTransform);

	m_pModel = dynamic_cast<CModel*>(pPlayer->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	return S_OK;
}

HRESULT CPlayerMove::Tick(const _double& TimeDelta)
{
	_vector vCamLook = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	_vector vCamRight = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
	_vector vDirection = XMVectorZero();

	vCamLook = XMVector3Normalize(XMVectorSetY(vCamLook, 0.f));

	while (true != m_Qmessage.empty())
	{
		switch (m_Qmessage.front())
		{
		case DIK_W:
			//vDirection.m128_f32[2] = 1.f;
			vDirection += vCamLook;
			break;

		case DIK_A:
			vDirection += -vCamRight;
			break;

		case DIK_S:
			vDirection += -vCamLook;
			break;

		case DIK_D:
			vDirection += vCamRight;
			break;
		}
		m_Qmessage.pop();
	}

	if (0 == XMVector3Length(vDirection).m128_f32[0])
		return S_OK;

	vDirection = XMVector3Normalize(vDirection);

	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	switch (pAction->Get_State())
	{
	case Client::CPlayerAction::STATE_DASH:
	case Client::CPlayerAction::STATE_LATK_1:
	case Client::CPlayerAction::STATE_LATK_2:
	case Client::CPlayerAction::STATE_LATK_3:
	case Client::CPlayerAction::STATE_LATK_4:
	case Client::CPlayerAction::STATE_HATK_1:
	case Client::CPlayerAction::STATE_HATK_2:
	case Client::CPlayerAction::STATE_HATK_3:
		if (true == m_pModel->isAbleChangeAnimation())
		{
			m_pTransform->Set_On_Navigation(true);
			m_pTransform->Go(vDirection, TimeDelta);
			pAction->Set_State(CPlayerAction::STATE_RUN);
		}
		break;

	case Client::CPlayerAction::STATE_IDLE:
		m_pTransform->Set_On_Navigation(true);
		m_pTransform->Go(vDirection, TimeDelta);
		pAction->Set_State(CPlayerAction::STATE_RUN);
		break;

	case Client::CPlayerAction::STATE_RUN:
		m_pTransform->Set_On_Navigation(true);
		m_pTransform->Go(vDirection, TimeDelta);
		pAction->Set_State(CPlayerAction::STATE_RUN);
		break;

	case Client::CPlayerAction::STATE_JUMP_LAND:
		m_pTransform->Set_On_Navigation(true);
		m_pTransform->Go(vDirection, TimeDelta);
		pAction->Set_State(CPlayerAction::STATE_RUN);
		break;

	case Client::CPlayerAction::STATE_JUMP:
		m_pTransform->Set_On_Navigation(false);
		m_pTransform->Go(vDirection, TimeDelta);
		break;

	case Client::CPlayerAction::STATE_DOUBLE_JUMP:
		m_pTransform->Set_On_Navigation(false);
		m_pTransform->Go(vDirection, TimeDelta);
		break;

	case Client::CPlayerAction::STATE_WHEEL:
		m_pTransform->Set_On_Navigation(true);
		m_pTransform->Go(vDirection, TimeDelta);
		break;
	}

	return S_OK;
}

CPlayerMove* CPlayerMove::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerMove* pInstance = new CPlayerMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMoveAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CPlayerMove* CPlayerMove::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CPlayerMove* pInstance = new CPlayerMove(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CMoveAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerMove::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pCamera);
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
