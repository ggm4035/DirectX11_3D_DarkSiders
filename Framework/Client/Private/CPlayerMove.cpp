
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

	m_pTransform = dynamic_cast<CPlayer*>(m_pOwner)->Get_Transform();
	if (nullptr == m_pTransform)
		return E_FAIL;
	Safe_AddRef(m_pTransform);

	m_pModel = dynamic_cast<CModel*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	return S_OK;
}

HRESULT CPlayerMove::Tick(const _double& TimeDelta)
{
	_vector vDirection = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	while (true != m_Qmessage.empty())
	{
		switch (m_Qmessage.front())
		{
		case DIK_W:
			vDirection.m128_f32[2] = 1.f;
			break;

		case DIK_S:
			vDirection.m128_f32[2] = -1.f;
			break;

		case DIK_D:
			vDirection.m128_f32[0] = 1.f;
			break;

		case DIK_A:
			vDirection.m128_f32[0] = -1.f;
			break;
		}
		m_Qmessage.pop();
	}

	if (0 == XMVector3Length(vDirection).m128_f32[0])
		return S_OK;

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
			m_pTransform->Go_OnNavigation(vDirection, TimeDelta);
			pAction->Set_State(CPlayerAction::STATE_RUN);
		}
		break;

	case Client::CPlayerAction::STATE_IDLE:
		m_pTransform->Go_OnNavigation(vDirection, TimeDelta);
		pAction->Set_State(CPlayerAction::STATE_RUN);
		break;

	case Client::CPlayerAction::STATE_RUN:
		m_pTransform->Go_OnNavigation(vDirection, TimeDelta);
		pAction->Set_State(CPlayerAction::STATE_RUN);
		break;

	case Client::CPlayerAction::STATE_JUMP_LAND:
		m_pTransform->Go_OnNavigation(vDirection, TimeDelta);
		pAction->Set_State(CPlayerAction::STATE_RUN);
		break;

	case Client::CPlayerAction::STATE_JUMP:
		m_pTransform->Go(vDirection, TimeDelta);
		break;

	case Client::CPlayerAction::STATE_DOUBLE_JUMP:
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
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}

	CBehavior::Free();
}
